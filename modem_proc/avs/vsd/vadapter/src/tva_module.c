/*
   Copyright (C) 2015-2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/src/tva_module.c#1 $
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

/* TDSCDMA APIs. */
#include "tds_ext_api.h"

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
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
#include "tva_if.h"
#include "tva_i.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define TVA_MAX_NUM_OF_SESSIONS_V ( 2 )
#define TVA_ENC_PKT_MISS_THRESHOLD ( 2 )

#define TVA_VOICE_SAMPLE_RATE_UNDEFINED_V ( 0 )
#define TVA_VOICE_SAMPLE_RATE_NB_V ( 8000 )
#define TVA_VOICE_SAMPLE_RATE_WB_V ( 16000 )

/* The frame size in microseconds that each voice processing threads 
 * (vptx, vprx, encoder, decoder, decoder pp) operates on.
 */
#define TVA_VOICE_FRAME_SIZE_US_V ( 20000 ) 

/* Offset definitions related to uplink processing 
 *
 * SOFT-VFR boundary aligns to HARD-VFR.
 * TDSCDMA reads uplink data froms DSM at offset of 1000 from VFR.
 *
 * Encoder delivers encoder packet at 18000 from VFR.
 * TVA triggers an UL timer to fire at 19000usec i.e. 2000usec prior to
 * TDSCDMA requesting same. we gave cushion of 2msec to ensure next SOFT-VFR 
 * do not inteferes with  with UL timer expiry.
 *
 * SOFT-VFR delay is adjusted while evaluating timer value.
 */
#define TVA_ENC_OFFSET_US ( 18000 )
#define TVA_PUT_UL_PACKET_INTO_DSM_OFFSET_US ( 19000 )
#define TVA_TDSCDMA_READS_UL_PACKET_OFFSET_US ( 1000 )


/* Offset definition related to downlink processing 
 *
 * SOFT-VFR boundary aligns to HARD-VFR.
 * TDSCDMA queue downlink data into DSM post every VFR, worst case delivery
 * by 3300usec.
 *
 * TVA triggers a timer to get DL from DSM at offset of 4300 i.e.
 * 1000usec prior to vocoder requesting same.
 * 
 * SOFT-VFR delay is adjusted  while evaluating timer value.
 */
#define TVA_GET_DL_PACKET_FROM_DSM_OFFSET_US ( 4300 )
#define TVA_DEC_REQ_OFFSET_US ( 5300 )
#define TVA_DEC_OFFSET_US ( 6300 )


/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

/* TVA address is set at initialization. */
char_t tva_my_dns[] =  "qcom.audio.tva";
static uint16_t tva_my_addr;
static uint32_t tva_apr_handle;

static apr_lock_t tva_int_lock;
static apr_lock_t tva_thread_lock;
static apr_event_t tva_control_event;

static apr_memmgr_type tva_heapmgr;
static uint8_t tva_heap_pool[ TVA_HEAP_SIZE_V ];

static apr_objmgr_t tva_objmgr;
static apr_objmgr_object_t tva_object_table[ TVA_MAX_OBJECTS_V ];

static tva_gating_control_t tva_gating_work_pkt_q;
static apr_list_t tva_nongating_work_pkt_q;
static apr_list_t tva_free_work_pkt_q;
static tva_work_item_t tva_work_pkts[ TVA_NUM_WORK_PKTS_V ];


static apr_event_t tva_work_event;
static apr_thread_t tva_thread;
static uint8_t tva_task_stack[ TVA_TASK_STACK_SIZE ];

static tva_session_object_t* tva_session_obj_list[TVA_MAX_NUM_OF_SESSIONS_V];
static apr_lock_t tva_global_lock;

static bool_t tva_is_eamr_enabled = FALSE;
static bool_t tva_is_initialized = FALSE; 

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *tva_ptr_Td =  NULL;
#endif

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void tva_int_lock_fn ( void )
{
  apr_lock_enter( tva_int_lock );
}

static void tva_int_unlock_fn ( void )
{
  apr_lock_leave( tva_int_lock );
}

static void tva_thread_lock_fn ( void )
{
  apr_lock_enter( tva_thread_lock );
}

static void tva_thread_unlock_fn ( void )
{
  apr_lock_leave( tva_thread_lock );
}

static void tva_signal_run ( void )
{
  apr_event_signal( tva_work_event );
}


/****************************************************************************
 * TVA CMDs & EVENTs PACKET QUEUING FUNCTIONS                               *
 ****************************************************************************/

/**
 * Queues the aprv2_packet_t (queues both incoming and pending packets to 
 * corresponding queues). In case of failure to queue a apr packet, packet 
 * will be automatically freed.
 */
static uint32_t tva_queue_apr_packet (
  aprv2_packet_t* packet,
  apr_list_t* cmd_q
)
{
  uint32_t rc = VS_EOK;
  tva_work_item_t* work_item = NULL;
  
  if( packet == NULL )
  {
    TVA_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    return VS_EFAILED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &tva_free_work_pkt_q, (apr_list_node_t**)&work_item );
    if ( NULL == work_item )
    {
      TVA_REPORT_FATAL_ON_ERROR(VS_ENORESOURCE);
      break;
    }

    work_item->packet = packet;
    rc = apr_list_add_tail( cmd_q, &work_item->link );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TVA: error in queuing APR packet: opcode[0x%08x], rc[0x%08x]", packet->opcode, rc );
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &tva_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      tva_signal_run( );
    }

    break;
  } /* for loop ends. */

  return rc;
}

static int32_t tva_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t rc = VS_EOK;

  rc = tva_queue_apr_packet( packet, &tva_nongating_work_pkt_q );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}



/****************************************************************************
 * TVA CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                  *
 ****************************************************************************/

static uint32_t tva_prepare_and_dispatch_cmd_packet (
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
    rc = __aprv2_cmd_alloc_ext( tva_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                APR_NULL_V, APR_NULL_V,
                                APR_NULL_V, APR_NULL_V, 
                                (uint32_t)session_context, cmd_id,
                                params_size, &packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TVA: APR packet allocation failed with rc=[0x%08x]", rc );
      break;
    }

    if ( ( params_size > 0 ) && ( cmd_params != NULL ) )
    {
      payload = APRV2_PKT_GET_PAYLOAD( void, packet);
      mmstd_memcpy( payload, params_size, cmd_params, params_size );
    }

    /* Queue the command packet for processing. */
    rc = tva_queue_apr_packet( packet, &tva_nongating_work_pkt_q );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TVA: APR packet queuing failed. rc[0x%08x]", rc );
      rc = __aprv2_cmd_free( tva_apr_handle, packet );
      TVA_REPORT_FATAL_ON_ERROR(rc);
    }

    break;
  }

  return rc;
}



/****************************************************************************
 * TVA OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES             *
 ****************************************************************************/

static void tva_pktexg_timer_cb (
  void* session_context
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = (tva_session_object_t*)session_context;

  if ( NULL == session_obj )
  {
    TVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return;
  }

  /* Queue DL buffer delivery event and schedule timer to UL buffer delivery. */
  if( TRUE == session_obj->is_timer_expiry_for_downlink )
  {
    session_obj->is_timer_expiry_for_downlink = FALSE;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: VSID[0x%08x] TVA_INTERNAL_EVENT_DELIVER_DL_BUFFER recieved", session_obj->vsid );
    rc = tva_prepare_and_dispatch_cmd_packet( 
         session_context, TVA_INTERNAL_EVENT_DELIVER_DL_BUFFER, NULL, 0 );
    TVA_REPORT_FATAL_ON_ERROR(rc);

    rc = apr_timer_start( session_obj->pktexg_timer,
                          ( TVA_PUT_UL_PACKET_INTO_DSM_OFFSET_US - TVA_GET_DL_PACKET_FROM_DSM_OFFSET_US) * 1000 );
    TVA_REPORT_FATAL_ON_ERROR(rc);
  }
  else
  {
    /* Queue UL buffer deivery event. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: VSID[0x%08x] TVA_INTERNAL_EVENT_DELIVER_UL_BUFFER recieved", session_obj->vsid );
    rc = tva_prepare_and_dispatch_cmd_packet( 
           session_obj, TVA_INTERNAL_EVENT_DELIVER_UL_BUFFER, NULL, 0 );
  }

  return;
}

static uint32_t tva_mem_alloc_object (
  uint32_t size,
  tva_object_t** ret_object
)
{
  int32_t rc;
  tva_object_t* tva_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the TVA object. */
    tva_obj = apr_memmgr_malloc( &tva_heapmgr, size );
    if ( tva_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "tva_mem_alloc_object(): Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the MVS object. */
    rc = apr_objmgr_alloc_object( &tva_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "tva_mem_alloc_object(): Out of objects, rc = (0x%08X)", rc );
      apr_memmgr_free( &tva_heapmgr, tva_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = tva_obj;

    /* Initialize the base MVS object header. */
    tva_obj->header.handle = objmgr_obj->handle;
    tva_obj->header.type = TVA_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = tva_obj;

  return APR_EOK;
}

static uint32_t tva_mem_free_object (
  tva_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &tva_objmgr, object->header.handle );
  apr_memmgr_free( &tva_heapmgr, object );

  return APR_EOK;
}


static uint32_t tva_create_session_object ( 
  tva_session_object_t** ret_session_obj )
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = tva_mem_alloc_object( sizeof( tva_session_object_t ),
                             ( ( tva_object_t** ) &session_obj ) );
  if ( rc )
  {
    *ret_session_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    session_obj->header.type = TVA_OBJECT_TYPE_ENUM_SESSION;
    session_obj->asid = SYS_MODEM_AS_ID_NONE;
    session_obj->vsid = TVA_VSID_UNDEFINED_V;

    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->data_lock );
    TVA_PANIC_ON_ERROR(rc);

    ( void ) voice_dsm_amr_q_init ( &session_obj->ul_queue, TVA_AMR_DSM_Q_LEN );
    ( void ) voice_dsm_amr_q_init ( &session_obj->dl_queue, TVA_AMR_DSM_Q_LEN );
    mmstd_memset ( &session_obj->ul_chan_state, 0, 
                   sizeof( session_obj->ul_chan_state ) );
    mmstd_memset ( &session_obj->dl_chan_state, 0, 
                   sizeof( session_obj->dl_chan_state ) );

    rc = apr_timer_create( &session_obj->pktexg_timer, tva_pktexg_timer_cb, session_obj );
    TVA_PANIC_ON_ERROR( rc );

    session_obj->tds_handle = APR_NULL_V;
    session_obj->is_tds_ready = FALSE;
    session_obj->vocoder_type = TVA_VOCODER_ID_UNDEFINED_V;
    session_obj->codec_mode = TVA_CODEC_MODE_UNDEFINED;
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

static uint32_t tva_create_simple_job_object (
  uint32_t parentjob_handle,
  tva_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  tva_simple_job_object_t* tva_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = tva_mem_alloc_object( sizeof( tva_simple_job_object_t ),
                             ( ( tva_object_t** ) &tva_obj ) );
  if ( rc )
  {
    *ret_job_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    tva_obj->header.type = TVA_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    tva_obj->context_handle = parentjob_handle;
    tva_obj->is_completed = 0;
  }

  *ret_job_obj = tva_obj;

  return APR_EOK;
}


/****************************************************************************
 * TVA TDSCDMA <> VS MAPPING  ROUTINES                                          *
 ****************************************************************************/

static uint32_t tva_map_vocamr_codec_mode_tdscdma_to_vs( 
  uint32_t tdscdma_codec_mode,
  uint8_t* vs_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( tdscdma_codec_mode )
  {
   case TDSCDMA_IVOCAMR_CODEC_MODE_0475:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0475;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_0515:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0515;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_0590:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0590;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_0670:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0670;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_0740:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0740;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_0795:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0795;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_1020:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_1020;
     break;

   case TDSCDMA_IVOCAMR_CODEC_MODE_1220:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_1220;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_codec_mode = TVA_CODEC_MODE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t tva_map_vocamrwb_codec_mode_tdscdma_to_vs( 
  uint32_t tdscdma_codec_mode,
  uint8_t* vs_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( tdscdma_codec_mode )
  {
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_0660:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_0660;
     break;
  
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_0885:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_0885;
     break;
  
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_1265:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1265;
     break;
  
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_1425:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1425;
     break;
    
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_1585:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1585;
     break;
    
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_1825:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1825;
     break;
    
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_1985:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1985;
     break;
   
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_2305:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_2305;
     break;
  
   case TDSCDMA_IVOCAMRWB_CODEC_MODE_2385:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_2385;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_codec_mode = TVA_CODEC_MODE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t tva_map_vocoder_type_tdscdma_to_vs(
  uint32_t tdscdma_vocoder_type
)
{
  uint32_t vs_media_id = 0xFFFFFFFF;

  switch ( tdscdma_vocoder_type )
  {
   case TDSCDMA_IVOCODER_ID_AMR:
     vs_media_id = (TRUE== tva_is_eamr_enabled)?VS_VOCEAMR_MEDIA_ID : VS_VOCAMR_MEDIA_ID;
     break;

   case TDSCDMA_IVOCODER_ID_AMRWB:
     vs_media_id = VS_VOCAMRWB_MEDIA_ID;
     break;
  }

  return vs_media_id;
}


static uint32_t tva_update_chan_state (
  voice_amr_chan_state_t* chan_state,
  uint32_t chan_class
)
{
  uint32_t rc = APR_EOK;

  switch ( chan_class )
  {
   case TDSCDMA_ICOMMON_CHAN_CLASS_TYPE_ABC:
     chan_state->has_chan_c = TRUE;
     /* fall through */

   case TDSCDMA_ICOMMON_CHAN_CLASS_TYPE_AB:
     chan_state->has_chan_b = TRUE;
     /* fall through */

   case TDSCDMA_ICOMMON_CHAN_CLASS_TYPE_A:
     chan_state->has_chan_a = TRUE;
     break;

   case TDSCDMA_ICOMMON_CHAN_CLASS_TYPE_NONE:
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
           "tva_update_chan_state(): Invalid channel class=(0x%08x)", 
           chan_class );
  }

  return rc;
}

static void tva_downlink_channel_data_available (
  uint8_t lc_id,
  uint8_t n_unit,
  void* session_context 
)
{
  tva_session_object_t* session_obj = NULL;
  session_obj = ( tva_session_object_t* ) session_context;

  for ( ;; )
  {
    if( lc_id == session_obj->dl_chan_state.lcc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "tva_downlink_channel_data_available(): logical channel C data "
             "available lc_id =(%d)", lc_id );
      break;
    }

    if( lc_id == session_obj->dl_chan_state.lcb )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "tva_downlink_channel_data_available(): logical channel B data "
             "available lc_id =(%d)", lc_id );
      break;
    }

    if( lc_id == session_obj->dl_chan_state.lca )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "tva_downlink_channel_data_available(): logical channel A data "
             "available lc_id =(%d)", lc_id );
      session_obj->is_dl_buffer_ready = TRUE;
      break;
    }

    break;
  }

  return;
}

/****************************************************************************
 * TVA COMMON ROUTINES                                                      *
 ****************************************************************************/


static void tva_publish_vocoder_state (
  tva_session_object_t* session_obj,
  uint32_t vocoder_state
)
{
  if( ( NULL != session_obj ) &&
      ( vocoder_state != session_obj->vocoder_state ) )
  {
    /* Publish vocoder state for voice call on a subscription. */
   session_obj->vocoder_state = vocoder_state;
   voicelog_event_report( session_obj->asid, session_obj->vsid,
                          session_obj->vocoder_state, VOICELOG_ICOMMON_NETWORK_ID_TDSCDMA,
                          tva_map_vocoder_type_tdscdma_to_vs( session_obj->vocoder_type ) );
  }

  return;
}


static uint32_t tva_log_vocoder_frame ( 
  tva_session_object_t* session_obj,
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
  log_cmd_param.network_id = VOICELOG_ICOMMON_NETWORK_ID_TDSCDMA;
  log_cmd_param.timestamp = 0;
  log_cmd_param.tap_point_id = VOICELOG_IPACKET_TAP_POINT_ID_VOICE_ADAPTER;

  log_cmd_param.media_id = tva_map_vocoder_type_tdscdma_to_vs ( session_obj->vocoder_type );
  log_cmd_param.frame_header = frameHeader;
  log_cmd_param.frame = frame;
  log_cmd_param.frame_size = frameSize;
  log_cmd_param.data_extension = NULL;

  /* Log data. */
  ( void ) voicelog_call ( VOICELOG_IPACKET_CMD_COMMIT_DATA, (void*)&log_cmd_param, 
                           sizeof( voicelog_ipacket_cmd_commit_data_t ) );

  return APR_EOK;
}



static uint32_t tva_get_dl_vocoder_packet ( 
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame = NULL;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  vsBuffer = session_obj->vs_write_buf;
  vsBuffer->media_id = tva_map_vocoder_type_tdscdma_to_vs( session_obj->vocoder_type );
  frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );

  rc = voice_amr_dl_processing( vsBuffer, &session_obj->dl_queue );
  if ( rc ) return rc;

  if ( ( VS_VOCAMR_MEDIA_ID == vsBuffer->media_id ) ||
       ( VS_VOCEAMR_MEDIA_ID == vsBuffer->media_id ) )
  {
    amrFrameHeader = (vs_vocamr_frame_header_t*)frame;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: (E)AMR: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrFrameHeader->frame_type, amrFrameHeader->codec_mode );
  }
  else if ( VS_VOCAMRWB_MEDIA_ID == vsBuffer->media_id )
  {
    amrwbFrameHeader = (vs_vocamrwb_frame_header_t*)frame;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: AMR-WB: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TVA: AMR-WB: GET_DL_PACKET: Invalid Vocoder type!!!" );
  }

  return rc;
}



static uint32_t tva_get_no_data_vocoder_packet ( 
  tva_session_object_t* session_obj
)
{
  uint8_t* frame = NULL;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;
  uint8_t codec_mode;
  vs_pktexg_buffer_t* vsBuffer = NULL;

  vsBuffer = session_obj->vs_write_buf;
  vsBuffer->media_id = tva_map_vocoder_type_tdscdma_to_vs( session_obj->vocoder_type );
  frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );

  if ( ( VS_VOCAMR_MEDIA_ID == vsBuffer->media_id ) ||
       ( VS_VOCEAMR_MEDIA_ID== vsBuffer->media_id ) )
  {
    tva_map_vocamr_codec_mode_tdscdma_to_vs( session_obj->codec_mode, &codec_mode);
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
    tva_map_vocamrwb_codec_mode_tdscdma_to_vs( session_obj->codec_mode, &codec_mode);
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


static uint32_t tva_deliver_ul_silence_vocoder_packet ( 
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t frame[VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH+1];
  uint32_t frameSize=0;
  uint32_t vs_media_id; uint8_t codec_mode;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  vs_media_id = tva_map_vocoder_type_tdscdma_to_vs( session_obj->vocoder_type );

  if ( ( vs_media_id == VS_VOCAMR_MEDIA_ID ) ||
       ( vs_media_id == VS_VOCEAMR_MEDIA_ID ) )
  {
    ( void ) tva_map_vocamr_codec_mode_tdscdma_to_vs( session_obj->codec_mode, &codec_mode );
    amrFrameHeader = (vs_vocamr_frame_header_t*)&frame[0];
    amrFrameHeader->frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD;
    amrFrameHeader->codec_mode = ( vs_vocamr_codec_mode_t ) codec_mode;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: (E)AMR: DELIVER_UL_PACKET: sending silence frame FrameType=(%d), CodecMode=(%d)",
           amrFrameHeader->frame_type, amrFrameHeader->codec_mode );
  }

  if( vs_media_id == VS_VOCAMRWB_MEDIA_ID )
  {
    ( void ) tva_map_vocamrwb_codec_mode_tdscdma_to_vs( session_obj->codec_mode, &codec_mode );
    amrwbFrameHeader = (vs_vocamrwb_frame_header_t*)&frame[0];
    amrwbFrameHeader->frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
    amrwbFrameHeader->codec_mode = ( vs_vocamrwb_codec_mode_t ) codec_mode;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: AMR-WB: DELIVER_UL_PACKET: sending silence frame FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
  }

  /* Get the silence frame vectors from voice utils. */
  rc = voice_util_get_homing_frame( vs_media_id, (void*)&frame[0], &frame[1],
                                    VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH, &frameSize );
  if ( rc ) return VS_EFAILED;

  /* Deliver the vocoder data to W-RLC via DSM queue. */
  rc = voice_amr_ul_processing( vs_media_id, &frame[0], &session_obj->ul_chan_state, &session_obj->ul_queue );

  ( void ) tva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                                  frame[0], &frame[1], frameSize );

  return rc;
}

static uint32_t tva_deliver_ul_vocoder_packet ( 
  tva_session_object_t* session_obj
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
           "TVA: (E)AMR: DELIVER_UL_PACKET: FrameType=(%d), CodecMode=(%d)",
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
           "TVA: AMR-WB: DELIVER_UL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
    rc = voice_amr_ul_processing( vs_buffer->media_id, frame,
                                  &session_obj->ul_chan_state, &session_obj->ul_queue );
    return VS_EOK;
  }


  return rc;
}


/****************************************************************************
 * TVA VS SESSION ROUTINES                                                  *
 ****************************************************************************/

static void tva_log_event_info(
  void* session_context,
  uint32_t event_id
)
{
  tva_session_object_t* session_obj = ( tva_session_object_t* ) session_context;

  if ( session_context == NULL ) return;

  switch( event_id )
  {
    case VS_COMMON_EVENT_CMD_RESPONSE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: ASID=(%d): VSID=(0x%08x): VS_COMMON_EVENT_CMD_RESPONSE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case VS_VOC_EVENT_NOT_READY:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_NOT_READY recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case VS_VOC_EVENT_READY:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_READY recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_OPERATING_MODE_UPDATE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TVA_INTERNAL_EVENT_DELIVER_DL_BUFFER:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "TVA: ASID=(%d): VSID=(0x%08x): TVA_INTERNAL_EVENT_DELIVER_DL_BUFFER recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TVA_INTERNAL_EVENT_DELIVER_UL_BUFFER:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "TVA: ASID=(%d): VSID=(0x%08x): TVA_INTERNAL_EVENT_DELIVER_UL_BUFFER recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICE_EVENT_REQUEST_START:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICE_EVENT_REQUEST_START recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICE_EVENT_REQUEST_STOP:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICE_EVENT_REQUEST_STOP recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICE_EVENT_SELECT_OWNER:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICE_EVENT_SELECT_OWNER recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICE_EVENT_REQUEST_SCR_MODE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICE_EVENT_REQUEST_SCR_MODE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case TDSCDMA_IVOICEL2_EVENT_VFR_NOTIFICATION:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "TVA: ASID=(%d): VSID=(0x%08x): TDSCDMA_IVOICEL2_EVENT_VFR_NOTIFICATION recieved",
             session_obj->asid, session_obj->vsid );
      break;

    default:
      break;
  }

  return;
}


static uint32_t tva_vs_event_cb(
 uint32_t event_id,
 void* params,
 uint32_t size,
 void* session_context
)
{
  uint32_t rc = APR_EOK;

  if ( tva_is_initialized == FALSE ) return APR_EOK;

  switch ( event_id )
  {
   case VS_COMMON_EVENT_CMD_RESPONSE:
   case VS_VOC_EVENT_NOT_READY:
   case VS_VOC_EVENT_READY:
   case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
    {
      (void) tva_log_event_info( session_context, event_id );
      rc = tva_prepare_and_dispatch_cmd_packet( session_context, event_id, params, size );
    }
    break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "tva_vs_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t tva_vs_set_voc_codec_mode (
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_rate_t param;
  vs_common_cmd_set_param_t setParamCmd;
  uint8_t codec_mode;

  switch ( session_obj->vocoder_type )
  {
     case TDSCDMA_IVOCODER_ID_AMR:
     {
       rc = tva_map_vocamr_codec_mode_tdscdma_to_vs( session_obj->codec_mode, &codec_mode );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "TVA: AMR: Unsupported CodecMode=(0x%08x)", session_obj->codec_mode );
       }
       else
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "TVA: AMR: CodecMode=(%d) sent to VS", codec_mode );
       }
     }
     break;

     case TDSCDMA_IVOCODER_ID_AMRWB:
     {
       rc = tva_map_vocamrwb_codec_mode_tdscdma_to_vs ( session_obj->codec_mode, &codec_mode );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "TVA: AMR-WB: Unsupported CodecMode=(0x%08x)", session_obj->codec_mode );
       }
       else
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "TVA: AMR-WB: CodecMode=(%d) sent to VS", codec_mode );
       }
     }
     break;

     default:
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "TVA: set codec mode for Unsupported vocoder=(0x%08x), ",
              session_obj->vocoder_type );
       return VS_EFAILED;
    }

  if ( NULL == session_obj->vs_handle )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "TVA: set codec mode VS session handle is not yet opened");
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
           "TVA: failed to set codec_mode=(%d), vocoder=(0x%08x)",
           session_obj->codec_mode, session_obj->vocoder_type );
  }

  return rc;
}


static uint32_t tva_vs_set_voc_dtx_mode (
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_dtx_v2_t param;
  vs_common_cmd_set_param_t setParamCmd;

  switch ( session_obj->vocoder_type )
  {
    case TDSCDMA_IVOCODER_ID_AMR:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: AMR dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    }
    break;

    case TDSCDMA_IVOCODER_ID_AMRWB:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: AMR-WB dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    }
    break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "tva_set_voc_dtx_mode(): Unsupported vocoder=(0x%08x), ",
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
           "TVA: failed to set dtx_mode=(%d), vocoder=(0x%08x)",
           session_obj->dtx_mode, session_obj->vocoder_type );
  }

  return rc;
}

static void tva_vs_set_voc_media_type (
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_media_id_t param;
  vs_common_cmd_set_param_t setParamCmd;

  param.media_id = tva_map_vocoder_type_tdscdma_to_vs( session_obj->vocoder_type );
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_VOC_MEDIA_ID;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TVA: failed to set mediaID[0x%08x]", session_obj->vocoder_type );
  }

  return;
}


static void tva_vs_set_voc_timing_offsets (
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_timing_offsets_t timingParam;
  vs_common_cmd_set_param_t setParamCmd;

  timingParam.enc_offset = TVA_ENC_OFFSET_US;
  timingParam.dec_req_offset = TVA_DEC_REQ_OFFSET_US;
  timingParam.dec_offset = TVA_DEC_OFFSET_US;
  timingParam.vfr_mode = 1;

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&timingParam;
  setParamCmd.param_id = VS_PARAM_VOC_TIMING_OFFSETS;
  setParamCmd.size = sizeof( timingParam );
  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "TVA: failed to set timing Offsets" );
  }

  return;
}

static uint32_t tva_vs_get_encoder_buffer (
  tva_session_object_t* session_obj
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
           "TVA: Failed to get read vs_buffer, vs_read_buf[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  return rc;
}


static uint32_t tva_vs_return_encoder_buffer (
  tva_session_object_t* session_obj
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
           "TVA: Failed to return read buffer, buffer[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  session_obj->vs_read_buf = NULL;
  return rc;
}

static uint32_t tva_vs_get_decoder_buffer (
  tva_session_object_t* session_obj
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
           "TVA: Failed to get vs_write_buf,  rc[0x%08x]", rc );
  }

  return rc;
}


static uint32_t tva_vs_put_decoder_buffer (
  tva_session_object_t* session_obj
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
           "TVA: Failed to return vs_write_buf[0x%08x], rc[0x%08x]", session_obj->vs_write_buf, rc );
  }

  session_obj->vs_write_buf = NULL;
  return APR_EOK;
}


static uint32_t tva_vs_open_vocoder_session (
 tva_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;

  /* Open VS session. */
  open_cmd.ret_handle = &session_obj->vs_handle;
  open_cmd.vsid = session_obj->vsid;
  open_cmd.network_id = VS_COMMON_NETWORK_ID_TDSCDMA;
  open_cmd.session_context = ( void* )session_obj;
  open_cmd.event_cb = tva_vs_event_cb;

  rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TVA: failed to open VS session rc = (0x%08x)", rc );
  }

  return rc;
}


static uint32_t tva_vs_enable_vocoder (
  tva_session_object_t* session_obj,
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

    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "TVA: Enabling VS!!!" );

    enableCmd.handle = session_obj->vs_handle;
    enableCmd.token = token;
    rc = vs_call( VS_VOC_CMD_ENABLE, &enableCmd, sizeof( enableCmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TVA: Failed to enable VS session, handle[0x%08x], rc[0x%08x]",
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


static uint32_t tva_vs_close_vocoder_session (
  tva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_close_t closeCmd;

  if ( session_obj->vs_handle == APR_NULL_V ) return VS_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "TVA: Closing VS!!!" );

  closeCmd.handle = session_obj->vs_handle;
  closeCmd.token = token;
  rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&closeCmd, sizeof( closeCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TVA: Failed to close VS session, handle[0x%08x], rc[0x%08x]",
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
 * TVA TDSCDMA SESSION ROUTINES                                                  *
 ****************************************************************************/

static uint32_t tva_tdscdma_event_cb(
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
           "tva_tdscdma_event_cb(): ASID=(%d) not supported", asid );
    return APR_EBADPARAM;
  }

  session_context = (void*)tva_session_obj_list[ asid ];

  switch ( event_id )
  {
    case TDSCDMA_IVOICE_EVENT_REQUEST_START:
    case TDSCDMA_IVOICE_EVENT_REQUEST_STOP:
    case TDSCDMA_IVOICE_EVENT_SELECT_OWNER:
    case TDSCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS:
    case TDSCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE:
    case TDSCDMA_IVOICE_EVENT_REQUEST_SCR_MODE:
    case TDSCDMA_IVOICEL2_EVENT_VFR_NOTIFICATION:
    {
      ( void ) tva_log_event_info( session_context, event_id );
      rc = tva_prepare_and_dispatch_cmd_packet( session_context, event_id, params, size );
    }
    break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "tva_tdscdma_event_cb(): Unsupported event (%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t tva_tdscdma_open_session (
  tva_session_object_t *session_obj
)
{
  uint32_t rc = APR_EOK;
  tdscdma_ivoice_cmd_open_t open_cmd;

  if( NULL == session_obj )
  {
    TVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  open_cmd.ret_handle = &session_obj->tds_handle;
  open_cmd.as_id = session_obj->asid;
  open_cmd.event_cb = tva_tdscdma_event_cb;

#ifdef FEATURE_SEGMENT_LOADING
  tva_ptr_Td = get_tdscdma_interface();
  if ( tva_ptr_Td != NULL )
  {
    rc = tds_ext_audio_api( TDSCDMA_IVOICE_CMD_OPEN, &open_cmd, sizeof( open_cmd ) );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "tva_tdscdma_open_session(): TDSCDMA SEGMENT NOT LOADED!!");
  }
#else
  rc = tds_ext_audio_api( TDSCDMA_IVOICE_CMD_OPEN, &open_cmd, sizeof( open_cmd ) );
#endif

  if ( ( APR_EOK != rc ) && ( APR_EUNSUPPORTED != rc ) ) 
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: Failed to open WCDMA session for asid[%d]) with rc[0x%08x]",
           open_cmd.as_id, rc );
  }
  else
  {
    rc = VS_EOK;
  }

  return rc;
}

static uint32_t tva_tdscdma_close_session (
  tva_session_object_t *session_obj
)
{
  uint32_t rc = APR_EOK;
  tdscdma_ivoice_cmd_close_t close_cmd;

  if( NULL == session_obj )
  {
    TVA_REPORT_FATAL_ON_ERROR(VS_EBADPARAM);
    return APR_EBADPARAM;
  }

  close_cmd.handle = session_obj->tds_handle;

#ifdef FEATURE_SEGMENT_LOADING
  tva_ptr_Td = get_tdscdma_interface();
  if ( tva_ptr_Td != NULL )
  {
    rc = tds_ext_audio_api( TDSCDMA_IVOICE_CMD_CLOSE, &close_cmd, sizeof( close_cmd ) );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "tva_tdscdma_close_session(): TDSCDMA SEGMENT NOT LOADED!!");
  }
#else
  rc = tds_ext_audio_api( TDSCDMA_IVOICE_CMD_CLOSE, &close_cmd, sizeof( close_cmd ) );
#endif

  if ( rc )
  {
   MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "TVA: failed to close WCDMA session for asid[%d], rc[0x%08x]",
          session_obj->asid, rc );
  }

  return rc;
}

static uint32_t tva_tdscdma_start_session (
  tva_session_object_t *session_obj
)
{
  uint32_t rc = APR_EOK;
  tdscdma_ivoice_cmd_start_t start_cmd;

  if( NULL == session_obj )
  {
    TVA_REPORT_FATAL_ON_ERROR(VS_EBADPARAM);
    return APR_EBADPARAM;
  }

  start_cmd.handle = session_obj->tds_handle;
  rc = tds_ext_audio_api ( TDSCDMA_IVOICE_CMD_START, &start_cmd, sizeof( start_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TVA: failed to start WCDMA session for asid[%d], rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

static uint32_t tva_tdscdma_stop_session (
  tva_session_object_t *session_obj
)
{
  uint32_t rc = APR_EOK;
  tdscdma_ivoice_cmd_stop_t stop_cmd;

  if( NULL == session_obj )
  {
    TVA_REPORT_FATAL_ON_ERROR(VS_EBADPARAM);
    return APR_EBADPARAM;
  }

  stop_cmd.handle = session_obj->tds_handle;
  rc = tds_ext_audio_api ( TDSCDMA_IVOICE_CMD_STOP, &stop_cmd, sizeof( stop_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TVA: failed to stop WCDMA session for asid[%d], rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

static uint32_t tva_tdscdma_set_vfr_notification (
  tva_session_object_t *session_obj
)
{
  uint32_t rc = APR_EOK;
  tdscdma_ivoicel2_cmd_set_vfr_notification_t set_vfr_cmd;

  if( NULL == session_obj )
  {
    TVA_REPORT_FATAL_ON_ERROR(VS_EBADPARAM);
    return APR_EBADPARAM;
  }

  set_vfr_cmd.as_id = session_obj->asid;
  set_vfr_cmd.enable_flag = session_obj->is_tds_ready;
  rc = tds_ext_audio_api( TDSCDMA_IVOICEL2_CMD_SET_VFR_NOTIFICATION, &set_vfr_cmd, sizeof(set_vfr_cmd) );
  if ( rc )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "TVA: failed to set VFR Notification enableFlag[%d], asid[%d], rc[0x%08x]",
           set_vfr_cmd.enable_flag, set_vfr_cmd.as_id, rc );
  }

  return rc;
}



static uint32_t tva_tdscdma_deregister_ul_logical_channels (
  tdscdma_ivoice_event_set_logical_channels_t* chan_info,
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* ul_queue = NULL;
  tdsl2_ul_service_register_type ul_service;
  tdscdma_ivoicel2_cmd_register_ul_service_t ul_srv_cmd;

  for( ;; )
  {
    chan_state = &session_obj->ul_chan_state;
    ul_queue = &session_obj->ul_queue;

#ifndef WINSIM
    /**
     * De-register logical channels if necessary.
     */
    ul_service.service = FALSE;

    /* deregister class A channel if necessary */
    if ( ( chan_state->has_chan_a == FALSE ) || 
         ( chan_state->lca != chan_info->lc_class_a ) )
    {
      if ( chan_state->lca != 0 )
      {
        ul_service.rlc_id[nchan] = chan_state->lca;
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
        ul_service.rlc_id[nchan] = chan_state->lcb;
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
        ul_service.rlc_id[nchan] = chan_state->lcc;
        nchan++;
        chan_state->lcc = 0;
      }
    }

    if( nchan > 0 )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->ul_queue );
       MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "DSM UL queue empty during de-registration" );
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
           "tva_tdscdma_deregister_ul_logical_channels(): AFTER: lca=(%d), lcb=(%d), "
           "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

    ul_service.nchan = nchan;
    ul_srv_cmd.as_id = session_obj->asid;
    ul_srv_cmd.service = &ul_service;

    rc  = tds_ext_audio_api( TDSCDMA_IVOICEL2_CMD_REGISTER_UL_SERVICE,
                             &ul_srv_cmd, sizeof( ul_srv_cmd ) );
    if( rc == TDSCDMA_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TDSCDMA L2 UL De-registration successful. Number of TDSCDMA L2 "
             "uplink logical channels, nchan = (%d).", nchan );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "tva_tdscdma_deregister_ul_logical_channels(): AFTER: lca=(%d), lcb=(%d), "
             "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "TDSCDMA L2 UL De-registration failed, rc = (0x%08X). Number of "
             "TDSCDMA L2 uplink logical channels, nchan = (%d).", rc, nchan );
      break;
    }

#endif

   break;
  }

  return rc;
}

static uint32_t tva_tdscdma_register_ul_logical_channels (
  tdscdma_ivoice_event_set_logical_channels_t* chan_info,
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* ul_queue = NULL;
  tdsl2_ul_service_register_type ul_service;
  tdscdma_ivoicel2_cmd_register_ul_service_t ul_srv_cmd;

  for( ;; )
  {
    chan_state = &session_obj->ul_chan_state;
    ul_queue = &session_obj->ul_queue;
  
#ifndef WINSIM
    /**
     * De-register logical channels if necessary.
     */
    ul_service.service = TRUE;

    /* Register class A channel if necessary */
    if ( ( chan_state->has_chan_a == TRUE ) && ( chan_info->lc_class_a > 0 ) )
    {
      chan_state->lca = chan_info->lc_class_a;
      ul_service.rlc_id[nchan] = chan_state->lca;
      ul_service.ul_wm_ptr[nchan] = &ul_queue->wm_a;
      nchan++;
    }
    
    /* Register class B channel if necessary */
    if ( ( chan_state->has_chan_b == TRUE ) && ( chan_info->lc_class_b > 0 ) )
    {
      chan_state->lcb = chan_info->lc_class_b;
      ul_service.rlc_id[nchan] = chan_state->lcb;
      ul_service.ul_wm_ptr[nchan] = &ul_queue->wm_b;
      nchan++;
    }
    
    /* Register class C channel if necessary */
    if ( ( chan_state->has_chan_c == TRUE ) && ( chan_info->lc_class_c > 0 ) )
    {
      chan_state->lcc = chan_info->lc_class_c;
      ul_service.rlc_id[nchan] = chan_state->lcc;
      ul_service.ul_wm_ptr[nchan] = &ul_queue->wm_c;
      nchan++;
    }
    
    if( nchan > 0 )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->ul_queue );
       MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "DSM UL queue empty during registration" );
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
           "tva_tdscdma_register_ul_logical_channels(): BEFORE: lca=(%d), lcb=(%d), "
           "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

    ul_service.nchan = nchan;
    ul_srv_cmd.as_id = session_obj->asid;
    ul_srv_cmd.service = &ul_service;

    rc  = tds_ext_audio_api( TDSCDMA_IVOICEL2_CMD_REGISTER_UL_SERVICE,
                             &ul_srv_cmd, sizeof( ul_srv_cmd ) );
    if( rc == TDSCDMA_EOK )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TDSCDMA L2 UL Registration successful. Number of TDSCDMA L2 "
             "uplink logical channels, nchan = (%d), la=(%d), lb=(%d), "
             "lc=(%d)", nchan, chan_info->lc_class_a, chan_info->lc_class_b,
             chan_info->lc_class_c );    
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "tva_tdscdma_register_ul_logical_channels(): AFTER: lca=(%d), lcb=(%d), "
             "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "TDSCDMA L2 UL Registration failed, rc = (0x%08X). Number of "
             "TDSCDMA L2 uplink logical channels, nchan = (%d).", rc, nchan );
      break;
    }

#endif

    break;
  }

  return rc;
}

static uint32_t tva_tdscdma_deregister_dl_logical_channels (
  tdscdma_ivoice_event_set_logical_channels_t* chan_info,
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* dl_queue = NULL;
  tdsl2_dl_service_register_type dl_service;
  tdscdma_ivoicel2_cmd_register_dl_service_t dl_srv_cmd;

  for( ;; )
  {
    chan_state = &session_obj->dl_chan_state;
    dl_queue = &session_obj->dl_queue;

#ifndef WINSIM
    /**
     * De-register logical channels if necessary
     */
    dl_service.service = FALSE;

    /* deregister class A channel if necessary */
    if ( ( chan_state->has_chan_a == FALSE ) || 
         ( chan_state->lca != chan_info->lc_class_a ) )
    {
      if ( chan_state->lca != 0 )
      {
        dl_service.rlc_id[nchan] = chan_state->lca;
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
       dl_service.rlc_id[nchan] = chan_state->lcb;
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
        dl_service.rlc_id[nchan] = chan_state->lcc;
        nchan++;
        chan_state->lcc = 0;
      }
    }

    if( nchan > 0 )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "DSM DL queue empty during de-registration" );
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
           "tva_tdscdma_deregister_dl_logical_channels(): BEFORE: lca=(%d), lcb=(%d), "
           "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

    dl_service.nchan = nchan;
    dl_srv_cmd.as_id = session_obj->asid;
    dl_srv_cmd.service = &dl_service;

    rc  = tds_ext_audio_api( TDSCDMA_IVOICEL2_CMD_REGISTER_DL_SERVICE,
                             &dl_srv_cmd, sizeof( dl_srv_cmd ) );
    if( rc == TDSCDMA_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TDSCDMA L2 DL De-registration successful. Number of TDSCDMA L2 "
             "downlink logical channels, nchan = (%d).", nchan );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "tva_tdscdma_deregister_dl_logical_channels(): AFTER: lca=(%d), lcb=(%d), "
             "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "TDSCDMA L2 DL De-registration failed, rc = (0x%08X). Number of "
             "TDSCDMA L2 downlink logical channels, nchan = (%d).", rc, nchan );
    }
#endif

    break;
  }

  return rc;
}

static uint32_t tva_tdscdma_register_dl_logical_channels (
  tdscdma_ivoice_event_set_logical_channels_t* chan_info,
  tva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* dl_queue = NULL;
  tdsl2_dl_service_register_type dl_service;
  tdscdma_ivoicel2_cmd_register_dl_service_t dl_srv_cmd;

  for( ;; )
  {
    chan_state = &session_obj->dl_chan_state;
    dl_queue = &session_obj->dl_queue;

#ifndef WINSIM
    /**
     * Register logical channels if necessary,
     */
    dl_service.service = TRUE;

    /* Register class A channel if necessary */
    if ( ( chan_state->has_chan_a == TRUE ) && ( chan_info->lc_class_a > 0 ) )
    {
      chan_state->lca = chan_info->lc_class_a;
      dl_service.rlc_id[nchan] = chan_state->lca;
      dl_service.dl_wm_ptr[nchan] = &dl_queue->wm_a;
      dl_service.context[nchan] = TRUE;
      dl_service.rlc_post_rx_func_ptr_para[nchan] = (void*)session_obj;
      dl_service.rlc_post_rx_proc_func_ptr[nchan] = tva_downlink_channel_data_available;
      nchan++;
    }

    /* Register class B channel if necessary */
    if ( ( chan_state->has_chan_b == TRUE ) && ( chan_info->lc_class_b > 0 ) )
    {
      chan_state->lcb = chan_info->lc_class_b;
      dl_service.rlc_id[nchan] = chan_state->lcb;
      dl_service.dl_wm_ptr[nchan] = &dl_queue->wm_b;
      dl_service.context[nchan] = TRUE;
      dl_service.rlc_post_rx_func_ptr_para[nchan] = (void*)session_obj;
      dl_service.rlc_post_rx_proc_func_ptr[nchan] = tva_downlink_channel_data_available;
      nchan++;
    }

    /* Register class C channel if necessary */
    if ( ( chan_state->has_chan_c == TRUE ) && ( chan_info->lc_class_c > 0 ) )
    {
      chan_state->lcc = chan_info->lc_class_c;
      dl_service.rlc_id[nchan] = chan_state->lcc;
      dl_service.dl_wm_ptr[nchan] = &dl_queue->wm_c;
      dl_service.context[nchan] = TRUE;
      dl_service.rlc_post_rx_func_ptr_para[nchan] = (void*)session_obj;
      dl_service.rlc_post_rx_proc_func_ptr[nchan] = tva_downlink_channel_data_available;
      nchan++;
    }

    if( nchan > 0 )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
         MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "DSM DL queue empty during registration" );
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
           "tva_tdscdma_register_dl_logical_channels(): BEFORE: lca=(%d), lcb=(%d), "
           "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

    dl_service.nchan = nchan;
    dl_srv_cmd.as_id = session_obj->asid;
    dl_srv_cmd.service = &dl_service;
    rc  = tds_ext_audio_api( TDSCDMA_IVOICEL2_CMD_REGISTER_DL_SERVICE,
                             &dl_srv_cmd, sizeof( dl_srv_cmd ) );
    if( rc == TDSCDMA_EOK )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TDSCDMA L2 DL Registration successful. Number of TDSCDMA L2 "
             "downlink logical channels, nchan = (%d), la=(%d), lb=(%d), "
             "lc=(%d)", nchan, chan_info->lc_class_a, chan_info->lc_class_b,
             chan_info->lc_class_c );

      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "tva_tdscdma_register_dl_logical_channels(): AFTER: lca=(%d), lcb=(%d), "
             "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "TDSCDMA L2 DL Registration failed, rc = (0x%08X). Number of "
             "TDSCDMA L2 downlink logical channels, nchan = (%d).", rc, nchan );
    }
#endif

    break;
  }

  return rc;
}


/****************************************************************************
 * TVA CMDs/EVENTs HANDLING ROUTINES                                        *
 ****************************************************************************/

static uint32_t tva_process_vs_cmd_response_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  tva_simple_job_object_t* simple_obj = NULL;
  vs_common_event_cmd_response_t* evt_params = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
   }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_common_event_cmd_response_t, packet );
    if( evt_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "TVA: cmd response recieved for cmd_id[0x%08x], token[0x%08x], status[0x%08X]",
           evt_params->cmd_id, evt_params->token, evt_params->status );

    simple_obj = ( tva_simple_job_object_t* ) evt_params->token;
    if ( simple_obj == NULL ) break;

    simple_obj->is_completed = TRUE;
    simple_obj->status = evt_params->status;

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_vs_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = TRUE;
    tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_READY );

    TVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_vs_not_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = FALSE;
    tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_NOT_READY );

    if( APR_NULL_V != session_obj->vs_handle )
    {
      flushCmd.handle = session_obj->vs_handle;
      flushCmd.enc_flush = TRUE;
      flushCmd.dec_flush = TRUE;
      rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
      TVA_REPORT_FATAL_ON_ERROR(rc);
    }

    TVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t tva_process_vs_eamr_rate_change_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  vs_voc_event_operating_mode_update_t* evt_params = NULL;
  tdscdma_ivoice_cmd_send_sample_rate_t sample_rate_cmd;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_voc_event_operating_mode_update_t, packet );
    if( evt_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    if( session_obj->vocoder_type != TDSCDMA_IVOCODER_ID_AMR )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "tva_process_vs_eamr_rate_change_event(): Not Applicable for "
             "vocoder_type=0x%08x", session_obj->vocoder_type );
      break;
    }

    switch ( evt_params->rx_mode )
    {
      case VS_VOC_BANDWIDTH_NB:
        sample_rate_cmd.sample_rate = TVA_VOICE_SAMPLE_RATE_NB_V;
        break;

      case VS_VOC_BANDWIDTH_WB:
        sample_rate_cmd.sample_rate = TVA_VOICE_SAMPLE_RATE_WB_V;
        break;

      default:
        sample_rate_cmd.sample_rate = TVA_VOICE_SAMPLE_RATE_UNDEFINED_V;
        break;
    }

    if( ( evt_params->rx_mode == VS_VOC_BANDWIDTH_NB ) ||
        ( evt_params->rx_mode == VS_VOC_BANDWIDTH_WB ) )
    {
      sample_rate_cmd.handle = session_obj->tds_handle;
      sample_rate_cmd.vocoder_id = session_obj->vocoder_type;
      rc = tds_ext_audio_api( TDSCDMA_IVOICE_CMD_SEND_SAMPLE_RATE,
                            &sample_rate_cmd, sizeof( sample_rate_cmd ) );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "TVA: sample rate change notification for asid[%d] rx_mode[%d] sample_rate[%d]",
             session_obj->asid, evt_params->rx_mode, sample_rate_cmd.sample_rate );
    }

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t tva_process_tdscdma_scr_mode_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  tdscdma_ivoice_event_request_scr_mode_t* evt_params = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( tdscdma_ivoice_event_request_scr_mode_t, packet );
    if ( evt_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    /* cache the new scr/dtx mode. */
    session_obj->dtx_mode = evt_params->enable_flag;
    ( void ) tva_vs_set_voc_dtx_mode ( session_obj );

    TVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_tdscdma_select_owner_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    /* FOR IRAT-HO Scenrios,request voice agent for resource grant. */
    if ( session_obj->is_resource_granted == FALSE )
    {
      requestResourceCmd.asid = session_obj->asid;
      requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_TDSCDMA;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                        sizeof(requestResourceCmd) );
      TVA_REPORT_FATAL_ON_ERROR(rc);

      tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
    }

    TVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t tva_process_tdscdma_codec_mode_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  tdscdma_ivoice_event_request_codec_mode_t* evt_params = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( tdscdma_ivoice_event_request_codec_mode_t, packet );
    if ( evt_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    /* cache the new codec mode. */
    session_obj->codec_mode = evt_params->codec_mode;
    ( void ) tva_vs_set_voc_codec_mode ( session_obj );

    TVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_tdscdma_l2_vfr_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  int64_t timing_offset_us = 0;
  uint32_t tds_sub_frame_count = 0;
  uint32_t tds_stmr_tick_count = 0;
  uint32_t tds_vfr_latency_in_us = 0;
  tdscdma_ivoicel2_event_vfr_notification_t* evt_params = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( tdscdma_ivoicel2_event_vfr_notification_t, packet );
    if ( evt_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    /* bits [28:16] represents the sub frame count from TDS. */
    tds_sub_frame_count = ( ( evt_params->tds_system_time & 0x1FFF0000 ) >> 16 );
    /* bits [15:0] represents the system tick for current TDS sub frame.
     * (512) ticks are equivalent to (50) usec. */
    tds_stmr_tick_count = ( evt_params->tds_system_time & 0x0000FFFF );
    tds_vfr_latency_in_us = ( uint32_t )( ( 1.0 * tds_stmr_tick_count ) / 10.24 ); 

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "TVA: L2_VFR_EVENT sub_frame_count[%d] stmr_tick_count[%d] vfr_latency_usec[%d]",
           tds_sub_frame_count, tds_stmr_tick_count, tds_vfr_latency_in_us );

    /* Avoid triggering UL/DL buffer delivery timer if TDSCDMA soft
     * VFR latency crosses threshold of 3000Usec. */
    if ( tds_vfr_latency_in_us > TVA_GET_DL_PACKET_FROM_DSM_OFFSET_US )
    {
      TVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );//
      break;
    }

    /* Trigger DL buffer delivery after adjusting VFR soft-VFR latency. */
    session_obj->is_timer_expiry_for_downlink = TRUE;
    timing_offset_us = TVA_GET_DL_PACKET_FROM_DSM_OFFSET_US - tds_vfr_latency_in_us;
    rc = apr_timer_start( session_obj->pktexg_timer, ( timing_offset_us*1000 ) );
    TVA_REPORT_FATAL_ON_ERROR(rc);

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_tdscdma_set_logical_channels_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  uint32_t chan_class;
  voice_amr_chan_state_t* chan_state = NULL;
  tva_session_object_t* session_obj = NULL;
  tdscdma_ivoice_event_set_logical_channels_t* evt_params = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( tdscdma_ivoice_event_set_logical_channels_t, packet );
    if ( evt_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    switch ( evt_params->direction )
    {
     /* Downlink logical channels. */
     case FALSE:
      {
        /* Update the channle state for DSM queue. */
        chan_class = evt_params->class_type;
        chan_state = &session_obj->dl_chan_state;
        rc = tva_update_chan_state( chan_state, chan_class );
        if( rc ) break;

        ( void ) tva_tdscdma_deregister_dl_logical_channels( evt_params, session_obj );
        ( void ) tva_tdscdma_register_dl_logical_channels( evt_params, session_obj );
      }
      break;
     
     /* Uplink logical channels. */
     case TRUE:
      {
        /* Update the channle state for DSM queue. */
        chan_class = evt_params->class_type;
        chan_state = &session_obj->ul_chan_state;
        rc = tva_update_chan_state( chan_state, chan_class );
        if( rc ) break;
        
        ( void ) tva_tdscdma_deregister_ul_logical_channels( evt_params, session_obj );
        ( void ) tva_tdscdma_register_ul_logical_channels( evt_params, session_obj );
      }
      break;
    }

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t tva_process_deliver_dl_buffer_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  tva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;
  uint8_t* frameHeader;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    if( session_obj->is_vs_ready == FALSE )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "tva_process_deliver_dl_buffer_event(): VS not ready "
           "discarding DL vocoder channel data" );
      break;
    }

    rc = tva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "tva_process_wcdma_dl_buf_available_event(): vs_write_buf is NULL" );
      break;
    }

    if( TRUE == session_obj->is_dl_buffer_ready )
    {
      /* Get the vocoder data from the DSM queue. */
      rc = tva_get_dl_vocoder_packet( session_obj );
    }
    else
    {
      rc = tva_get_no_data_vocoder_packet( session_obj );
    }
    session_obj->is_dl_buffer_ready = FALSE;

    if ( rc ) 
    {
      TVA_REPORT_FATAL_ON_ERROR(rc);
      break;
    }

    frameHeader = ( (uint8_t*)session_obj->vs_write_buf + sizeof(vs_pktexg_buffer_t) );
    ( void ) tva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                                    *frameHeader, frameHeader+1, session_obj->vs_write_buf->frame_size-1 );

    flushCmd.handle = session_obj->vs_handle;
    flushCmd.enc_flush = FALSE; flushCmd.dec_flush = TRUE;
    rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
    TVA_REPORT_FATAL_ON_ERROR(rc);

    /* Pass the vocoder buffer to VS from rendering. */
    rc = tva_vs_put_decoder_buffer( session_obj );

    /*TBD add logging for DL packet. */
    //Add mechanism to send noData packet if data not available from WCDMA

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_deliver_ul_buffer_event ( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  uint8_t* frameHeader;
  tva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    if(TRUE == session_obj->is_vs_ready)
    {
      rc = tva_vs_get_encoder_buffer ( session_obj );
    }
	
    if ( NULL != session_obj->vs_read_buf )
    {
      session_obj->enc_packet_miss_count = 0;
      ( void ) tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RUNNING );
      ( void ) tva_deliver_ul_vocoder_packet( session_obj );
      frameHeader = ( (uint8_t*)session_obj->vs_read_buf + sizeof(vs_pktexg_buffer_t) );
      ( void ) tva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                                      *frameHeader, frameHeader+1, session_obj->vs_read_buf->frame_size-1 );
      ( void ) tva_vs_return_encoder_buffer( session_obj );
    }
    else if( session_obj->enc_packet_miss_count >= TVA_ENC_PKT_MISS_THRESHOLD )
    {
      ( void ) tva_deliver_ul_silence_vocoder_packet( session_obj );
    }
    else
    {
      session_obj->enc_packet_miss_count++;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "TVA: UL vocoder Packet not transfered to WCDMA is_vs_ready[%d] readBuf[0x%08x]",
             session_obj->is_vs_ready, session_obj->vs_read_buf );
    }

    break;
  }

  rc = __aprv2_cmd_free( tva_apr_handle, packet );
  TVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t tva_process_tdscdma_vocoder_start_event( 
  tva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  tva_session_object_t* session_obj = NULL;
  tdscdma_ivoice_event_request_start_t* evt_params = NULL;
  tva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj= ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      rc = APR_EUNEXPECTED;
      TVA_REPORT_FATAL_ON_ERROR ( rc );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( tdscdma_ivoice_event_request_start_t, packet );
    if( evt_params == NULL )
    {
      rc = APR_EUNEXPECTED;
      TVA_REPORT_FATAL_ON_ERROR ( rc );
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == TVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "tva_process_tdscdma_vocoder_start_event() - configured vocoder "
             "id = (0x%08x), requested vocoder id = (0x%08x)",
             session_obj->vocoder_type, evt_params->vocoder_id );

      if ( session_obj->is_tds_ready == TRUE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "tva_process_tdscdma_vocoder_start_event() - TDSCDMA already requested");
        break;
      }

      if ( evt_params->vocoder_id != TDSCDMA_IVOCODER_ID_NONE )
      {
        /* During Inter-frequency HHO scenrios, reuse the vocoder_type 
           set initially by WRRC. */
        session_obj->vocoder_type = evt_params->vocoder_id;
      }

      /* Vaildate the vocoder_type and check if the Logical channel are 
         configured to adapter by WRRC, this is to avoid multiple start/stop 
         from WL1 during add/drop state machine. */
      if ( ( session_obj->vocoder_type == TVA_VOCODER_ID_UNDEFINED_V ) ||
           ( session_obj->ul_chan_state.has_chan_a == FALSE ) ||
           ( session_obj->dl_chan_state.has_chan_a == FALSE ) )
      {
         MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                "tva_process_tdscdma_vocoder_start_event() - Invalid vocoder "
                "type = (0x%08x) configurred, ul_dsm_state=%d, dl_dsm_state=%d",
                session_obj->vocoder_type, session_obj->ul_chan_state.has_chan_a,
                session_obj->dl_chan_state.has_chan_a );
         break;
      }

      /* Update the TDSCDMA state. */
      session_obj->is_tds_ready = TRUE;
      session_obj->enc_packet_miss_count = 0;
      if ( session_obj->is_resource_granted == FALSE )
      {
        /* Request for voice resource. */
        requestResourceCmd.asid = session_obj->asid;
        requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_TDSCDMA;
        rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                          sizeof(requestResourceCmd) );
        TVA_REPORT_FATAL_ON_ERROR(rc);

        tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
        break;
      }

      rc = tva_vs_open_vocoder_session( session_obj );
      TVA_PANIC_ON_ERROR(rc);

      /* Set cached vocoder properties. */
      ( void ) tva_vs_set_voc_media_type( session_obj );
      ( void ) tva_vs_set_voc_dtx_mode ( session_obj );
      ( void ) tva_vs_set_voc_codec_mode ( session_obj );
      ( void ) tva_vs_set_voc_timing_offsets( session_obj );

      /* Create the Simple job object to track VS_ENABLE. */
      rc = tva_create_simple_job_object( session_obj->header.handle,
             ( tva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      rc = tva_vs_enable_vocoder( session_obj, simple_obj );
    }
    else
    {
     simple_obj = &ctrl->rootjob_obj->simple_job;
     if( simple_obj->is_completed != TRUE )
     {
       rc = APR_EPENDING;
       break;
     }

     ( void ) tva_tdscdma_start_session( session_obj );
     ( void ) tva_tdscdma_set_vfr_notification( session_obj );
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
    ( void ) tva_mem_free_object ( ( tva_object_t*) simple_obj );
  }

  if( session_obj != NULL )
  {
    TVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t tva_process_tdscdma_vocoder_stop_event( 
 tva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  tva_session_object_t* session_obj = NULL;
  tva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( tva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( rc );
      rc = APR_EUNEXPECTED;
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == TVA_GATING_CMD_STATE_EXECUTE )
    {
      /* If the TRRC de-registeres the logical channles, update the session
         vocoder type as undefined, to ignore multiple start/stopn from TL1
         during add/drop state machine. */
      if ( ( session_obj->ul_chan_state.has_chan_a == FALSE ) &&
           ( session_obj->dl_chan_state.has_chan_a == FALSE ) )
      {
        session_obj->vocoder_type = TVA_VOCODER_ID_UNDEFINED_V;
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "tva_process_tdscdma_vocoder_stop_event(): Invalidate configured "
               "type = (0x%08x), ul_dsm_state=%d, dl_dsm_state=%d",
               session_obj->vocoder_type, session_obj->ul_chan_state.has_chan_a,
               session_obj->dl_chan_state.has_chan_a );
      }

      if ( FALSE == session_obj->is_tds_ready )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "tdscdma_vocoder_stop_event(PROCESS): Vocoder not started yet" );
        break;
      }

      /* Update the TDSCDMA state. */
      session_obj->is_tds_ready = FALSE;

      /* Create the Simple job object to track CVD setup. */
      rc = tva_create_simple_job_object( session_obj->header.handle,
             ( tva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      rc = tva_vs_close_vocoder_session( session_obj, simple_obj );
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

    if ( APR_EOK == rc ) 
    {
      /* Send voice resource released event to voice agent.
       * "REQUEST_STOP sent by T-RRC without sending REQUEST_START" 
       * This can happen if T-RRC called SELECT_OWNER during interRAT 
       * handover start. However the handover failed afterwards. So, T-RRC 
       * did not call REQUEST_START, instead directly called REQUEST_STOP 
       * to indicate TVA that it no longer required vocoder. 
       */
       session_obj->is_resource_granted = FALSE;
       releaseResourceCmd.asid = session_obj->asid;
       releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_TDSCDMA;
       rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                         sizeof(releaseResourceCmd) );
       TVA_REPORT_FATAL_ON_ERROR(rc);

       tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );

      /* Send STOP and VFR Disable notfication to TDSCDMA. */
      ( void ) tva_tdscdma_stop_session ( session_obj );
      ( void ) tva_tdscdma_set_vfr_notification ( session_obj );

      /* Free CMD/EVT packet memory. */
      ( void ) tva_mem_free_object ( ( tva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    TVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}

static uint32_t tva_va_event_cb (
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
      rc = tva_prepare_and_dispatch_cmd_packet( NULL, event_id, params, size );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "TVA: Unsupported EventId=(%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t tva_process_resource_grant_cmd( 
 tva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  tva_session_object_t* session_obj = NULL;
  vagent_ivoice_event_resource_grant_t* event_params = NULL;
  tva_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_grant_t, packet );
    if( event_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = tva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == TVA_GATING_CMD_STATE_EXECUTE )
    {
      session_obj->is_resource_granted = TRUE;
      session_obj->vsid = event_params->vsid;
      tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_GRANTED );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_GRANT recieved",
             session_obj->asid, session_obj->vsid );

      if ( session_obj->is_tds_ready == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "tva_process_resource_grant_cmd() - Traffic start request not "
             "available from TDSCDMA" );
        break;
      }

      tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_GRANTED );

      rc = tva_vs_open_vocoder_session( session_obj );
      TVA_PANIC_ON_ERROR(rc);

      /* Set cached vocoder properties. */
      ( void ) tva_vs_set_voc_media_type( session_obj );
      ( void ) tva_vs_set_voc_dtx_mode ( session_obj );
      ( void ) tva_vs_set_voc_codec_mode ( session_obj );
      ( void ) tva_vs_set_voc_timing_offsets( session_obj );

      /* Create the Simple job object to track VS_ENABLE. */
      rc = tva_create_simple_job_object( session_obj->header.handle,
             ( tva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      rc = tva_vs_enable_vocoder( session_obj, simple_obj );
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
      ( void ) tva_tdscdma_start_session( session_obj );
      ( void ) tva_tdscdma_set_vfr_notification( session_obj );
      ( void ) tva_mem_free_object ( ( tva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    TVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t tva_process_resource_revoke_cmd( 
 tva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  tva_session_object_t* session_obj = NULL;
  vagent_ivoice_event_resource_revoke_t* event_params = NULL;
  tva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if ( packet == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_revoke_t, packet );
    if( event_params == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = tva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      TVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    TVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == TVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "TVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_REVOKE recieved",
             session_obj->asid, session_obj->vsid );

      /* Create the Simple job object to track VS_DISABLE. */
      rc = tva_create_simple_job_object( session_obj->header.handle,
             ( tva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj= &ctrl->rootjob_obj->simple_job;

      rc = tva_vs_close_vocoder_session( session_obj, simple_obj );
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
      releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_TDSCDMA;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                        sizeof(releaseResourceCmd) );
      TVA_REPORT_FATAL_ON_ERROR(rc);

      ( void ) tva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );
      ( void ) tva_mem_free_object ( ( tva_object_t*) simple_obj );
    }

    break;
  }

  if ( session_obj != NULL )
  {
    TVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


/****************************************************************************
 * NONGATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/

static void tva_task_process_nongating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  tva_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;

  while( apr_list_remove_head( &tva_nongating_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &tva_free_work_pkt_q, &work_item->link );

    switch( packet->opcode )
    {
     /**
      * Handling routine for nongating work-items should take of release the
      * memory allocated for the CMD/EVENT packets.
      */
     case TDSCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE:
       rc = tva_process_tdscdma_codec_mode_event( packet );
       break;

     case TDSCDMA_IVOICE_EVENT_REQUEST_SCR_MODE:
       rc = tva_process_tdscdma_scr_mode_event( packet );
       break;

     case TDSCDMA_IVOICE_EVENT_SELECT_OWNER:
       rc = tva_process_tdscdma_select_owner_event( packet );
       break;

     case TDSCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS:
       rc = tva_process_tdscdma_set_logical_channels_event( packet );
       break;

     case TDSCDMA_IVOICEL2_EVENT_VFR_NOTIFICATION:
       rc = tva_process_tdscdma_l2_vfr_event( packet );
       break;

     case TVA_INTERNAL_EVENT_DELIVER_DL_BUFFER:
       rc = tva_process_deliver_dl_buffer_event( packet );
       break;

     case TVA_INTERNAL_EVENT_DELIVER_UL_BUFFER:
       rc = tva_process_deliver_ul_buffer_event( packet );
       break;

     case VS_COMMON_EVENT_CMD_RESPONSE:
       rc = tva_process_vs_cmd_response_event( packet );
       break;

     case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
       rc = tva_process_vs_eamr_rate_change_event( packet );
       break;

     case VS_VOC_EVENT_READY:
       rc = tva_process_vs_ready_event( packet );
       break;

     case VS_VOC_EVENT_NOT_READY:
       rc = tva_process_vs_not_ready_event( packet );
       break;

     default:
       /* Add remaining work items to the gating work queue. */
       rc = tva_queue_apr_packet ( packet, &tva_gating_work_pkt_q.cmd_q );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "TVA: NON-GATING: cmd pkt queuing failed rc[0x%08x]", rc );
         rc = __aprv2_cmd_free( tva_apr_handle, packet );
         TVA_REPORT_FATAL_ON_ERROR(rc);
       }
       break;
    }
  }

  return;
}

/****************************************************************************
 * GATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/
 
static void tva_task_process_gating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  tva_work_item_t* work_item;
  tva_gating_control_t* ctrl = &tva_gating_work_pkt_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
      case TVA_GATING_CMD_STATE_FETCH:
      {
        /* Fetch the next gating command to execute. */
        rc = apr_list_remove_head( &ctrl->cmd_q,
                                     ( ( apr_list_node_t** ) &work_item ) );
        if ( rc ) return;

        if ( work_item->packet == NULL )

        {
          TVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
        }

        ctrl->packet = work_item->packet;
        ctrl->state = TVA_GATING_CMD_STATE_EXECUTE;
        work_item->packet = NULL;
        ( void ) apr_list_add_tail( &tva_free_work_pkt_q, &work_item->link );
      }
      break;

      case TVA_GATING_CMD_STATE_EXECUTE:
      case TVA_GATING_CMD_STATE_CONTINUE:
      {
        if ( TVA_GATING_CMD_STATE_EXECUTE == ctrl->state )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "TVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) STARTED",
                 ctrl->packet->opcode  );
        }

        /* For Supported request_id, handler should take care of releasing 
         * memory allocated for packets. */
        switch ( ctrl->packet->opcode  )
        {
          case TDSCDMA_IVOICE_EVENT_REQUEST_START:
            rc = tva_process_tdscdma_vocoder_start_event( ctrl );
            break;
 
          case TDSCDMA_IVOICE_EVENT_REQUEST_STOP:
            rc = tva_process_tdscdma_vocoder_stop_event( ctrl );
            break;

          case VAGENT_IVOICE_EVENT_RESOURCE_GRANT:
            rc = tva_process_resource_grant_cmd( ctrl );
            break;
 
          case VAGENT_IVOICE_EVENT_RESOURCE_REVOKE:
            rc = tva_process_resource_revoke_cmd( ctrl );
            break;

          default:
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "TVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) UNSUPPORTED",
                   ctrl->packet->opcode  );
            break;
        }

         /* Evaluate the pending command completion status. */
        switch ( rc )
        {
          case APR_EOK:
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                   "TVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) ENDED",
                   ctrl->packet->opcode  );
            rc = __aprv2_cmd_free( tva_apr_handle, ctrl->packet );
            TVA_REPORT_FATAL_ON_ERROR(rc);

            ctrl->packet = NULL;
            /* The current command is finished so fetch the next command. */
            ctrl->state = TVA_GATING_CMD_STATE_FETCH;
          }
          break;

          case APR_EPENDING:
            /**
             * Assuming the current pending command control routine returns
             * APR_EPENDING the overall progress stalls until one or more
             * external events or responses are received.
             */
            ctrl->state = TVA_GATING_CMD_STATE_CONTINUE;
            /**
             * Return from here so as to avoid unecessary looping till reponse
             * is recived.
             */
            return;

          default:
            TVA_PANIC_ON_ERROR( APR_EUNEXPECTED );
            break;
         }
       }
       break;

      default:
        TVA_PANIC_ON_ERROR( rc );
        break;
    }/* switch case ends. */
  }/* for loop ends. */

  return;
}

/****************************************************************************
 * TVA TASK ROUTINES                                                        *
 ****************************************************************************/

static int32_t tva_run ( void )
{
  tva_task_process_nongating_work_items( );
  tva_task_process_gating_work_items( );

  return APR_EOK;
}

static int32_t tva_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &tva_work_event );
  apr_event_signal( tva_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( tva_work_event );
    if ( rc ) break;

    tva_run( );
  }

  apr_event_destroy( tva_work_event );
  apr_event_signal( tva_control_event );

  return APR_EOK;
}


/****************************************************************************
 * TVA BOOT-UP and POWER-DOWN ROUTINES                                      *
 ****************************************************************************/

static uint32_t tva_gating_control_init (
  tva_gating_control_t* p_ctrl
)
{
  uint32_t rc = APR_EOK;

  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q,
                         tva_thread_lock_fn, tva_thread_unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  p_ctrl->state = TVA_GATING_CMD_STATE_FETCH;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return APR_EOK;
}  /* end of tva_gating_control_init () */

static uint32_t tva_gating_control_destroy (
  tva_gating_control_t* p_ctrl
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

static int32_t tva_init ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index;
  RCINIT_INFO info_handle = NULL;
  RCINIT_PRIO priority = 0;
  unsigned long stack_size = 0;
  voicecfg_cmd_get_bool_item_t item;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "====== tva_init(): ======" );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &tva_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &tva_thread_lock );
    apr_event_create( &tva_control_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &tva_heapmgr, ( ( void* ) &tva_heap_pool ),
                          sizeof( tva_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = tva_object_table;
    params.total_bits = TVA_HANDLE_TOTAL_BITS_V;
    params.index_bits = TVA_HANDLE_INDEX_BITS_V;
    params.lock_fn = tva_int_lock_fn;
    params.unlock_fn = tva_int_unlock_fn;
    rc = apr_objmgr_construct( &tva_objmgr, &params );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &tva_free_work_pkt_q, 
                           tva_int_lock_fn, tva_int_unlock_fn );
    for ( index = 0; index < TVA_NUM_WORK_PKTS_V; ++index )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &tva_work_pkts[index] );
      tva_work_pkts[index].packet = NULL;
      rc = apr_list_add_tail( &tva_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &tva_work_pkts[index] ) );
    }

    rc = apr_list_init_v2( &tva_nongating_work_pkt_q,
                           tva_int_lock_fn, tva_int_unlock_fn );
  }

  { /* Initialize gating work pkt queue. */
    rc = tva_gating_control_init( &tva_gating_work_pkt_q );
  }

  { /* Initialize the global session lock. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &tva_global_lock );
    TVA_PANIC_ON_ERROR( rc );
  }

  { /* Create the TVA task worker thread. */
#if ( !defined AVS_MPSS_TEST )
    info_handle = rcinit_lookup( TVA_TASK_NAME );
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
           "tva_init(): TVA task not registered with RCINIT" );
      priority = TVA_TASK_PRIORITY;
      stack_size = TVA_TASK_STACK_SIZE;
    }

    if ( ( priority > 255 ) || ( stack_size == 0 ) ) 
    {
      ERR_FATAL( "tva_init(): Invalid priority: %d or stack size: %d",
                 priority, stack_size, 0 );
    }

    rc = apr_thread_create( &tva_thread, TVA_TASK_NAME, TASK_PRIORITY(priority),
                            tva_task_stack, stack_size, 
                            tva_worker_thread_fn , NULL );
    TVA_PANIC_ON_ERROR( rc );

    apr_event_wait( tva_control_event );
  }

  tva_is_initialized = TRUE;

  item.id = VOICECFG_CFG_IS_TDS_EAMR_ENABLED;
  item.ret_value = FALSE;
  rc =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );
  if ( ! rc  && item.ret_value == TRUE )
  {
    tva_is_eamr_enabled = TRUE;
  }

  return rc;
}


static int32_t tva_postinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;
  vagent_ivoice_cmd_register_t registerCmd;

  rc = __aprv2_cmd_register2(
         &tva_apr_handle, tva_my_dns, sizeof( tva_my_dns ), 0,
         tva_isr_dispatch_fn, NULL, &tva_my_addr );
  TVA_PANIC_ON_ERROR(rc);

  /* Initialize the mapping info and open onex voice instance. */
  for( index = 0; index < TVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* Create and initialize TVA session object. */
    rc =  tva_create_session_object ( &tva_session_obj_list[index] );
    TVA_PANIC_ON_ERROR( rc );
    tva_session_obj_list[index]->asid = ( sys_modem_as_id_e_type ) index;

    /* Open TDSCDMA voice session instance. */
    rc = tva_tdscdma_open_session( tva_session_obj_list[index] );
    TVA_PANIC_ON_ERROR (rc);
  }

  /* Registring with voice agent for resource. */
  registerCmd.client_id = VAGENT_VOICE_CLIENT_TDSCDMA;
  registerCmd.client_cb = tva_va_event_cb;
  registerCmd.client_data = NULL;
  rc = vagent_call( VAGENT_IVOICE_CMD_REGISTER, (void*)&registerCmd, sizeof(registerCmd) );
  TVA_PANIC_ON_ERROR (rc);

  return rc;
}


static int32_t tva_predeinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;
  
  /* Close onex Session instance. */
  for( index = 0; index < TVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* TVA CLOSE's CDMA session instance for all available ASID. */
    ( void ) tva_tdscdma_close_session( tva_session_obj_list[index] );
  
    /* Free TVA session object for all VSID. */
    ( void ) apr_timer_destroy ( tva_session_obj_list[index]->pktexg_timer );
    ( void ) apr_lock_destroy( tva_session_obj_list[index]->data_lock );
    ( void ) tva_mem_free_object( (tva_object_t*)tva_session_obj_list[index] );
  }

  return rc;
}


static int32_t tva_deinit ( void )
{
  uint32_t rc = APR_EOK;

  tva_is_initialized = FALSE;

  apr_event_signal_abortall( tva_work_event );
  apr_event_wait( tva_control_event );


  /* Release gating control structures */
  ( void ) tva_gating_control_destroy( &tva_gating_work_pkt_q );

  /* Release work queue */
  ( void ) apr_list_destroy( &tva_free_work_pkt_q );
  ( void ) apr_list_destroy( &tva_nongating_work_pkt_q );


  /* Deinitialize the object handle table. */
  ( void ) apr_objmgr_destruct( &tva_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  ( void ) apr_event_destroy( tva_control_event );
  ( void ) apr_lock_destroy( tva_int_lock );
  ( void ) apr_lock_destroy( tva_thread_lock );
  ( void ) apr_lock_destroy( tva_global_lock );

  return rc;
}


TVA_EXTERNAL uint32_t tva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  if ( ( FALSE == tva_is_initialized ) && ( DRV_CMDID_INIT != cmd_id ) )
  {
    TVA_REPORT_FATAL_ON_ERROR( APR_ENOSERVICE );
    return APR_ENOSERVICE;
  }

  switch ( cmd_id )
  {
    case DRV_CMDID_INIT:
      rc = tva_init( );
      break;

    case DRV_CMDID_POSTINIT:
      rc = tva_postinit( );
      break;

    case DRV_CMDID_PREDEINIT:
      rc = tva_predeinit( );
      break;

    case DRV_CMDID_DEINIT:
      rc = tva_deinit( );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "tva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;
}

