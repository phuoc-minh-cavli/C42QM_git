/*
   Copyright (C) 2015-2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/src/cva_module.c#1 $
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
#include "rcinit.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_memmgr.h"

/* 1x APIs. */
#include "onex_voice_if.h"

/* VSD APIs. */
#include "drv_api.h"
#include "vs_task.h"
#include "vs.h"
#include "voicelog_if.h"
#include "voicelog_utils.h"
#include "vagent.h"

/* SELF APIs. */
#include "cva_if.h"
#include "cva_i.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Currently voice agent support max two subscription and max two VSID. */
#define CVA_MAX_NUM_OF_SESSIONS_V ( 2 )

#define CVA_MAX_VOC_FRAME_LENGTH ( 40 )

/* ENC/DEC Offset definitions.
 *
 * A CDMA encoding/decoding cycle is divided into 16 PCG slots start from 
 * PCG-0 to PCG-15 each of 1.25msec length. PCG-0 alligns to HARD-VFR.
 *
 * Uplink callback is always triggered at PCG-0 to pick up the previously 
 * delivered encoder packet, hence encoder offset is 18000 with 2msec cushion.
 * 
 * Downlink callback is triggered bases on the RC configuration.
 *
 * RC3 configuration: DL frame available in cycle (X) and is always delivered for
 * voice decoding during start of cycle (X+1).
 *
 * RC3 configuration aka 1X advanced processing: DL frame if available early can be delivered
 * for voice decoding as early as PCG-2 (~2500-3750 from VFR) during same cycle.
 * DEC_REQ_OFFSET as 2000 ensures that frame available early during cycle X+1 will not 
 * flush frame from cycle X.
 */
#define CVA_ENC_OFFSET_US ( 18000 )
#define CVA_DEC_REQ_OFFSET_US ( 2000 )
#define CVA_DEC_OFFSET_US ( 3000 )

/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

/* CVA address is set at initialization. */
char_t cva_my_dns[] =  "qcom.audio.cva";
static uint16_t cva_my_addr;
static uint32_t cva_apr_handle;

static apr_lock_t cva_int_lock;
static apr_lock_t cva_thread_lock;
static apr_event_t cva_control_event;

static apr_memmgr_type cva_heapmgr;
static uint8_t cva_heap_pool[ CVA_HEAP_SIZE_V ];

static apr_objmgr_t cva_objmgr;
static apr_objmgr_object_t cva_object_table[ CVA_MAX_OBJECTS_V ];

static cva_gating_control_t cva_gating_work_pkt_q;
static apr_list_t cva_nongating_work_pkt_q;
static apr_list_t cva_free_work_pkt_q;
static cva_work_item_t cva_work_pkts[ CVA_NUM_WORK_PKTS_V ];


static apr_event_t cva_work_event;
static apr_thread_t cva_thread;
static uint8_t cva_task_stack[ CVA_TASK_STACK_SIZE ];

static cva_session_object_t* cva_session_obj_list[CVA_MAX_NUM_OF_SESSIONS_V];
static apr_lock_t cva_global_lock;

static bool_t cva_is_initialized = FALSE; 


/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void cva_int_lock_fn ( void )
{
  apr_lock_enter( cva_int_lock );
}

static void cva_int_unlock_fn ( void )
{
  apr_lock_leave( cva_int_lock );
}

static void cva_thread_lock_fn ( void )
{
  apr_lock_enter( cva_thread_lock );
}

static void cva_thread_unlock_fn ( void )
{
  apr_lock_leave( cva_thread_lock );
}

static void cva_signal_run ( void )
{
  apr_event_signal( cva_work_event );
}

/****************************************************************************
 * CVA CMDs & EVENTs PACKET QUEUING FUNCTIONS                               *
 ****************************************************************************/

/**
 * Queues the aprv2_packet_t (queues both incoming and pending packets to 
 * corresponding queues). In case of failure to queue a apr packet, packet 
 * will be automatically freed.
 */
static uint32_t cva_queue_apr_packet (
  aprv2_packet_t* packet,
  apr_list_t* cmd_q
)
{
  uint32_t rc = VS_EOK;
  cva_work_item_t* work_item = NULL;
  
  if( packet == NULL )
  {
    CVA_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    return VS_EFAILED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &cva_free_work_pkt_q, (apr_list_node_t**)&work_item );
    if ( NULL == work_item )
    {
      CVA_REPORT_FATAL_ON_ERROR(VS_ENORESOURCE);
      break;
    }

    work_item->packet = packet;
    rc = apr_list_add_tail( cmd_q, &work_item->link );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVA: error in queuing APR packet: opcode[0x%08x], rc[0x%08x]", packet->opcode, rc );
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &cva_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      cva_signal_run( );
    }

    break;
  } /* for loop ends. */

  return rc;
}


static int32_t cva_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t rc = VS_EOK;

  rc = cva_queue_apr_packet( packet, &cva_nongating_work_pkt_q );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}


/****************************************************************************
 * CVA CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                  *
 ****************************************************************************/

static uint32_t cva_prepare_and_dispatch_cmd_packet (
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
    rc = __aprv2_cmd_alloc_ext( cva_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                APR_NULL_V, APR_NULL_V,
                                APR_NULL_V, APR_NULL_V, 
                                (uint32_t)session_context, cmd_id,
                                params_size, &packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVA: APR packet allocation failed with rc=[0x%08x]", rc );
      break;
    }

    if ( ( params_size > 0 ) && ( cmd_params != NULL ) )
    {
      payload = APRV2_PKT_GET_PAYLOAD( void, packet);
      mmstd_memcpy( payload, params_size, cmd_params, params_size );
    }

    /* Queue the command packet for processing. */
    rc = cva_queue_apr_packet( packet, &cva_nongating_work_pkt_q );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVA: APR packet queuing failed. rc[0x%08x]", rc );
      rc = __aprv2_cmd_free( cva_apr_handle, packet );
      CVA_REPORT_FATAL_ON_ERROR(rc);
    }

    break;
  }

  return rc;
}


/****************************************************************************
 * CVA OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES             *
 ****************************************************************************/

static uint32_t cva_mem_alloc_object (
  uint32_t size,
  cva_object_t** ret_object
)
{
  int32_t rc;
  cva_object_t* cva_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the CVA object. */
    cva_obj = apr_memmgr_malloc( &cva_heapmgr, size );
    if ( cva_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_mem_alloc_object(): Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the MVS object. */
    rc = apr_objmgr_alloc_object( &cva_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "cva_mem_alloc_object(): Out of objects, rc = (0x%08X)", rc );
      apr_memmgr_free( &cva_heapmgr, cva_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = cva_obj;

    /* Initialize the base MVS object header. */
    cva_obj->header.handle = objmgr_obj->handle;
    cva_obj->header.type = CVA_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = cva_obj;

  return APR_EOK;
}

static uint32_t cva_mem_free_object (
  cva_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &cva_objmgr, object->header.handle );
  apr_memmgr_free( &cva_heapmgr, object );

  return APR_EOK;
}


static uint32_t cva_create_session_object ( 
  cva_session_object_t** ret_session_obj )
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cva_mem_alloc_object( sizeof( cva_session_object_t ),
                             ( ( cva_object_t** ) &session_obj ) );
  if ( rc )
  {
    *ret_session_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    session_obj->header.type = CVA_OBJECT_TYPE_ENUM_SESSION;
    session_obj->asid = SYS_MODEM_AS_ID_NONE;
    session_obj->vsid = CVA_VSID_UNDEFINED_V;

    session_obj->is_resource_granted = FALSE;
    session_obj->vocoder_state = VOICELOG_IEVENT_VOCODER_STATE_RELEASED;
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->data_lock );
    CVA_PANIC_ON_ERROR(rc);

    session_obj->onex_handle = APR_NULL_V;
    session_obj->is_onex_ready = FALSE;
    session_obj->vocoder_type = CVA_VOCODER_ID_UNDEFINED_V;
    session_obj->avg_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
    session_obj->min_rate = CVA_MIN_MAX_RATE_UNDEFINED;
    session_obj->max_rate = CVA_MIN_MAX_RATE_UNDEFINED;
    session_obj->dtx_mode = FALSE;

    session_obj->vs_handle = APR_NULL_V;
    session_obj->vs_read_buf = NULL;
    session_obj->vs_write_buf = NULL;
    session_obj->is_vs_ready = FALSE;
  }

  *ret_session_obj = session_obj;

  return APR_EOK;
}

static uint32_t cva_create_simple_job_object (
  uint32_t parentjob_handle,
  cva_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  cva_simple_job_object_t* cva_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cva_mem_alloc_object( sizeof( cva_simple_job_object_t ),
                             ( ( cva_object_t** ) &cva_obj ) );
  if ( rc )
  {
    *ret_job_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    cva_obj->header.type = CVA_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    cva_obj->context_handle = parentjob_handle;
    cva_obj->is_completed = 0;
  }

  *ret_job_obj = cva_obj;

  return APR_EOK;
}

/****************************************************************************
 * CVA ONEX <> VS MAPPING  ROUTINES                                          *
 ****************************************************************************/

static uint32_t cva_map_voc_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   uint8_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = 0x00;
       break;

     case ONEX_VOC_COP_1:
       *vs_enc_rate = 0x01;
       break;

     case ONEX_VOC_COP_2:
       *vs_enc_rate = 0x02;
       break;

     case ONEX_VOC_COP_3:
       *vs_enc_rate = 0x03;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = 0x04;
       break;

     case ONEX_VOC_COP_5:
       *vs_enc_rate = 0x05;
       break;

     case ONEX_VOC_COP_6:
       *vs_enc_rate = 0x06;
       break;

     case ONEX_VOC_COP_7:
       *vs_enc_rate = 0x07;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }

  return rc;
}

static uint32_t cva_map_voc_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  uint8_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = 0; //FRAME_BLANK_RATE ENUM
     break;

   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = 1; //FRAME_EIGTH_RATE ENUM
     break;

   case ONEX_VOC_FRAME_RATE_QUARTER:
     *vs_frame_rate = 2; //FRAME_QUATER_RATE ENUM
     break;

   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = 3; //FRAME_HALF_RATE ENUM
     break;

   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = 4; //FRAME_FULL_RATE ENUM
     break;

   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = 14; //FRAME_ERASURE_RATE ENUM
     break;

   case ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL:
     *vs_frame_rate = 255; //FRAME_EIGH_RATE_NONCRITICAL ENUM
     break;

   default:
     rc = APR_EBADPARAM;
     break;
  }

  return rc;
}


static uint32_t cva_map_vocoder_type_onex_to_vs(
  uint32_t onex_vocoder_type
)
{
  uint32_t vs_media_id = CVA_VOCODER_ID_UNDEFINED_V;

  switch ( onex_vocoder_type )
  {
   case ONEX_VOC_ID_EVRCA:
     vs_media_id = VS_VOCEVRCA_MEDIA_ID;
     break;

   case ONEX_VOC_ID_EVRCB:
     vs_media_id = VS_VOCEVRCB_MEDIA_ID;
     break;

   case ONEX_VOC_ID_EVRCNW:
    vs_media_id = VS_VOCEVRCNW_MEDIA_ID;
    break;

   case ONEX_VOC_ID_EVRCNW2K:
    vs_media_id = VS_VOCEVRCNW2K_MEDIA_ID;
    break;

   case ONEX_VOC_ID_EVRCWB:
     vs_media_id = VS_VOCEVRCWB_MEDIA_ID;
     break;

   case ONEX_VOC_ID_QCELP13K:
     vs_media_id = VS_VOCQCELP13K_MEDIA_ID;
     break;

   default:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "cva_map_vocoder_type_onex_to_vs(): vocoder type (0x%08x) "
               "unsupported by CVA", onex_vocoder_type );
        break;
  }

  return vs_media_id;
}

static uint32_t cva_map_voc_frame_rate_vs_to_onex(
  uint8_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case 0:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case 1:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case 2:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_QUARTER;
     break;

   case 3:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;
  
   case 4:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case 14:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;

   case 255:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL;
     break;

   default:
     rc = APR_EBADPARAM;
     break;
  }

  return rc;
}


/****************************************************************************
 * CVA COMMON ROUTINES                                                      *
 ****************************************************************************/

static void cva_publish_vocoder_state (
  cva_session_object_t* session_obj,
  uint32_t vocoder_state
)
{
  if( ( NULL != session_obj ) &&
      ( vocoder_state != session_obj->vocoder_state ) )
  {
    /* Publish vocoder state for voice call on a subscription. */
   session_obj->vocoder_state = vocoder_state;
   voicelog_event_report( session_obj->asid, session_obj->vsid,
                          session_obj->vocoder_state, VOICELOG_ICOMMON_NETWORK_ID_CDMA,
                          cva_map_vocoder_type_onex_to_vs( session_obj->vocoder_type ) );
  }

  return;
}

static uint32_t cva_log_vocoder_frame ( 
  cva_session_object_t* session_obj,
  uint32_t log_code,
  uint8_t* frameHeader,
  onex_vocoder_buffer_t* onex_buffer
)
{
  voicelog_ipacket_cmd_commit_data_t log_cmd_param; 

  log_cmd_param.version = VOICELOG_IPACKET_VOCODER_DATA_VERSION_V;
  log_cmd_param.log_code = log_code;
  log_cmd_param.vsid = session_obj->vsid;
  log_cmd_param.network_id = VOICELOG_ICOMMON_NETWORK_ID_CDMA;
  log_cmd_param.timestamp = 0;
  log_cmd_param.tap_point_id = VOICELOG_IPACKET_TAP_POINT_ID_VOICE_ADAPTER;

  log_cmd_param.media_id = cva_map_vocoder_type_onex_to_vs ( onex_buffer->vocoder_id );
  log_cmd_param.frame_header = *frameHeader;
  log_cmd_param.frame = onex_buffer->frame;
  log_cmd_param.frame_size = onex_buffer->size;
  log_cmd_param.data_extension = NULL;

  /* Log data. */
  ( void ) voicelog_call ( VOICELOG_IPACKET_CMD_COMMIT_DATA, (void*)&log_cmd_param, 
                           sizeof( voicelog_ipacket_cmd_commit_data_t ) );

  return APR_EOK;
}


static bool_t cva_validate_cdma_vocoder_id (
  uint32_t vocoder_id
)
{
  bool_t rc = FALSE;

  switch ( vocoder_id )
  {
   case ONEX_VOC_ID_EVRCA:
   case ONEX_VOC_ID_EVRCB:
   case ONEX_VOC_ID_EVRCNW:
   case ONEX_VOC_ID_EVRCNW2K:
   case ONEX_VOC_ID_EVRCWB:
   case ONEX_VOC_ID_QCELP13K:
     rc = TRUE;
     break;

   default:
     break;
  }

  return rc;
}


/****************************************************************************
 * CVA VS SESSION ROUTINES                                                  *
 ****************************************************************************/

static void cva_log_event_info(
  void* session_context,
  uint32_t event_id
)
{
  cva_session_object_t* session_obj = ( cva_session_object_t* ) session_context;

  if ( session_context == NULL ) return;

  switch( event_id )
  {
    case VS_COMMON_EVENT_CMD_RESPONSE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID=(%d): VSID=(0x%08x): VS_COMMON_EVENT_CMD_RESPONSE recieved",
            session_obj->asid, session_obj->vsid );
      break;

    case VS_VOC_EVENT_NOT_READY:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_NOT_READY recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case VS_VOC_EVENT_READY:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_READY recieved",
            session_obj->asid, session_obj->vsid );
      break;

    case ONEX_VOC_EVENT_RQST_START:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID=(%d): VSID=(0x%08x): ONEX_VOC_EVENT_RQST_START recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case ONEX_VOC_EVENT_RQST_STOP:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID=(%d): VSID=(0x%08x): ONEX_VOC_EVENT_RQST_STOP recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case ONEX_VOC_EVENT_RQST_DTX_MODE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: ASID=(%d): VSID=(0x%08x): ONEX_VOC_EVENT_RQST_DTX_MODE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case ONEX_VOC_EVENT_RQST_AVG_ENC_RATE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: ASID=(%d): VSID=(0x%08x): ONEX_VOC_EVENT_RQST_AVG_ENC_RATE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    case ONEX_VOC_EVENT_RQST_MIN_MAX_RATE:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: ASID=(%d): VSID=(0x%08x): ONEX_VOC_EVENT_RQST_MIN_MAX_RATE recieved",
             session_obj->asid, session_obj->vsid );
      break;

    default:
      break;
  }

  return;
}

static uint32_t cva_vs_event_cb(
 uint32_t event_id,
 void* params,
 uint32_t size,
 void* session_context
)
{
  uint32_t rc = APR_EOK;

  if ( cva_is_initialized == FALSE ) return APR_EOK;

  switch ( event_id )
  {
   case VS_COMMON_EVENT_CMD_RESPONSE:
   case VS_VOC_EVENT_NOT_READY:
   case VS_VOC_EVENT_READY:
     (void) cva_log_event_info( session_context, event_id );
      rc = cva_prepare_and_dispatch_cmd_packet( 
             session_context, event_id, params, size );
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "cva_vs_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t cva_vs_set_voc_min_max_rate (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_minmax_rate_t param;
  vs_common_cmd_set_param_t setParamCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) return VS_EOK;

  rc |= cva_map_voc_frame_rate_onex_to_vs( session_obj->min_rate, &param.min_rate );
  rc |= cva_map_voc_frame_rate_onex_to_vs( session_obj->max_rate, &param.max_rate );
  if ( rc )
  {
    CVA_REPORT_FATAL_ON_ERROR(rc);
    return VS_EOK;
  }

  switch ( session_obj->vocoder_type )
  {
    case ONEX_VOC_ID_EVRCA:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-A: min_rate=(%d), max_rate=(%d) sent to VS",
              param.min_rate, param.max_rate );
      break;

    case ONEX_VOC_ID_EVRCB:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-B: min_rate=(%d), max_rate=(%d) sent to VS",
             param.min_rate, param.max_rate );
      break;

    case ONEX_VOC_ID_EVRCNW:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-NW: min_rate=(%d), max_rate=(%d) sent to VS",
              param.min_rate, param.max_rate );
      break;

    case ONEX_VOC_ID_EVRCWB:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "CVA: EVRC-WB: min_rate=(%d), max_rate=(%d) sent to VS",
              param.min_rate, param.max_rate );
       break;

    case ONEX_VOC_ID_EVRCNW2K:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "CVA: EVRC-NW2K: min_rate=(%d), max_rate=(%d) sent to VS",
              param.min_rate, param.max_rate );
      break;

    case ONEX_VOC_ID_QCELP13K:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: QCELP-13K: min_rate=(%d), max_rate=(%d) sent to VS",
              param.min_rate, param.max_rate );
      break;
  }

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_MINMAX_FRAME_RATE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: set minmax rate failed vocoder[0x%08x] min_rate[0x%08x] max_rate[0x%08x]",
           session_obj->vocoder_type, session_obj->min_rate, session_obj->max_rate );
  }

  return rc;
}


static uint32_t cva_vs_set_voc_enc_rate (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_rate_t param;
  vs_common_cmd_set_param_t setParamCmd;


  if ( APR_NULL_V == session_obj->vs_handle ) return VS_EOK;

  rc = cva_map_voc_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &param.rate );
  if( rc )
  {
    CVA_REPORT_FATAL_ON_ERROR(rc);
    return VS_EOK;
  }

  switch ( session_obj->vocoder_type )
  {
    case ONEX_VOC_ID_EVRCA:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-A enc_rate_modulation=(%d) sent to VS", param.rate );
      break;

    case ONEX_VOC_ID_EVRCB:
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "CVA: EVRC-B enc_rate=(%d) sent to VS", param.rate );
       break;

    case ONEX_VOC_ID_EVRCNW:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-NW enc_rate=(%d) sent to VS", param.rate );
      break;

    case ONEX_VOC_ID_EVRCWB:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-WB enc_rate=(%d) sent to VS", param.rate );
      break;

    case ONEX_VOC_ID_EVRCNW2K:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-NW2K enc_rate=(%d) sent to VS", param.rate );
      break;

    case ONEX_VOC_ID_QCELP13K:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: QCELP-13K enc_rate=(%d) sent to VS", param.rate );
      break;
  }

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_RATE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: Incorrect enc_rate[0x%08x] for vocoder[0x%08x]",
           session_obj->avg_enc_rate, session_obj->vocoder_type );
  }

  return rc;
}

static uint32_t cva_vs_set_voc_dtx_mode (
  cva_session_object_t* session_obj
)
{

  uint32_t rc = APR_EOK;
  vs_param_enc_dtx_v2_t param;
  vs_common_cmd_set_param_t setParamCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) return VS_EOK;

  switch ( session_obj->vocoder_type )
  {
    case ONEX_VOC_ID_EVRCA:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "CVA: EVRC-A dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
      break;

    case ONEX_VOC_ID_EVRCB:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "CVA: EVRC-B dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
     break;
 
    case ONEX_VOC_ID_EVRCNW:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "CVA: EVRC-NW dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
     break;
 
    case ONEX_VOC_ID_EVRCWB:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "CVA: EVRC-WB dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
     break;
 
    case ONEX_VOC_ID_EVRCNW2K:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "CVA: EVRC-NW2K dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
     break;
 
    case ONEX_VOC_ID_QCELP13K:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "CVA: QCELP-13K dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
     break;
 
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_set_voc_dtx_mode(): Unsupported vocoder=(0x%08x)",
             session_obj->vocoder_type );
      break;
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
           "CVA: failed to set dtx_mode=(%d), vocoder=(0x%08x)",
           session_obj->dtx_mode, session_obj->vocoder_type );
  }

  return rc;
}

static void cva_vs_set_voc_media_type (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_media_id_t param;
  vs_common_cmd_set_param_t setParamCmd;

  param.media_id = cva_map_vocoder_type_onex_to_vs( session_obj->vocoder_type );
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_VOC_MEDIA_ID;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: failed to set mediaID[0x%08x]", session_obj->vocoder_type );
  }

  return;
}


static void cva_vs_set_voc_timing_offsets (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_timing_offsets_t timingParam;
  vs_common_cmd_set_param_t setParamCmd;

  timingParam.enc_offset = CVA_ENC_OFFSET_US;
  timingParam.dec_req_offset = CVA_DEC_REQ_OFFSET_US;
  timingParam.dec_offset =CVA_DEC_OFFSET_US;
  timingParam.vfr_mode = 1;

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&timingParam;
  setParamCmd.param_id = VS_PARAM_VOC_TIMING_OFFSETS;
  setParamCmd.size = sizeof( timingParam );
  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "CVA: failed to set timing Offsets" );
  }

  return;
}

static uint32_t cva_vs_get_encoder_buffer (
  cva_session_object_t* session_obj
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
           "CVA: Failed to get read vs_buffer, vs_read_buf[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  return rc;
}


static uint32_t cva_vs_return_encoder_buffer (
  cva_session_object_t* session_obj
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
           "CVA: Failed to return read buffer, buffer[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  session_obj->vs_read_buf = NULL;
  return rc;
}

static uint32_t cva_vs_get_decoder_buffer (
  cva_session_object_t* session_obj
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
           "CVA: Failed to get vs_write_buf,  rc[0x%08x]", rc );
  }

  return rc;
}


static uint32_t cva_vs_put_decoder_buffer (
  cva_session_object_t* session_obj
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
           "CVA: Failed to return vs_write_buf[0x%08x], rc[0x%08x]", session_obj->vs_write_buf, rc );
  }

  session_obj->vs_write_buf = NULL;
  return APR_EOK;
}


static uint32_t cva_vs_open_vocoder_session (
 cva_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;

  /* Open VS session. */
  open_cmd.ret_handle = &session_obj->vs_handle;
  open_cmd.vsid = session_obj->vsid;
  open_cmd.network_id = VS_COMMON_NETWORK_ID_CDMA;
  open_cmd.session_context = ( void* )session_obj;
  open_cmd.event_cb = cva_vs_event_cb;

  rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: failed to open VS session rc = (0x%08x)", rc );
  }

  return rc;
}


static uint32_t cva_vs_enable_vocoder (
  cva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_enable_t enableCmd;

  /* Update Vocoder Session Number for vocoder logging purpose. */
  ( void ) voicelog_session_update ( session_obj->vsid,
                                     VOICELOG_INFO_ENUM_VOCODER_SESSION_NUMBER);

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "CVA: Enabling VS!!!" );

  enableCmd.handle = session_obj->vs_handle;
  enableCmd.token = token;
  rc = vs_call( VS_VOC_CMD_ENABLE, &enableCmd, sizeof( enableCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: Failed to enable VS session, handle[0x%08x], rc[0x%08x]",
           session_obj->vs_handle, rc );
  }
  else
  {
    rc = APR_EPENDING;
  }

  return rc;
}


static uint32_t cva_vs_close_vocoder_session (
  cva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_close_t closeCmd;

  if ( session_obj->vs_handle == APR_NULL_V ) return VS_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "CVA: Closing VS!!!" );

  closeCmd.handle = session_obj->vs_handle;
  closeCmd.token = token;
  rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&closeCmd, sizeof( closeCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: Failed to close VS session, handle[0x%08x], rc[0x%08x]",
           session_obj->vs_handle, rc );
  }
  else
  {
    session_obj->vs_handle = APR_NULL_V;
    rc = APR_EPENDING;
  }

  return rc;
}


static uint32_t cva_deliver_ul_vocoder_packet ( 
  cva_session_object_t* session_obj,
  onex_vocoder_buffer_t* onex_buffer
)
{
  uint32_t rc = APR_EOK;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_pktexg_buffer_t* vs_buffer = session_obj->vs_read_buf;
  uint8_t* frameHeader = NULL;
  uint32_t frame_rate;

  for( ;; )
  {
    if ( vs_buffer->media_id != cva_map_vocoder_type_onex_to_vs(session_obj->vocoder_type) )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVA: DELIVER_UL_PACKET: Invalid MediaId=(0x%08x)", vs_buffer->media_id );
      break;
    }

    frameHeader = ( (uint8_t*) vs_buffer ) + sizeof( vs_pktexg_buffer_t );
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_voc_frame_rate_vs_to_onex ( *frameHeader, &frame_rate );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVA: DELIVER_UL_PACKET: Unsupported FrameRate[%d] for MediaID[0x%08x]",
             *frameHeader, vs_buffer->media_id );
      break;
    }

    switch ( session_obj->vocoder_type )
    {
      case ONEX_VOC_ID_EVRCA:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-A: DELIVER_UL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 3-half, 4-full, 14-erasure)", *frameHeader );
        break;

      case ONEX_VOC_ID_EVRCB:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-B: DELIVER_UL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure)", 
               *frameHeader );
        break;

      case ONEX_VOC_ID_EVRCNW:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-NW: DELIVER_UL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure, 255 - eighth non critical)",
               *frameHeader );
        break;

      case ONEX_VOC_ID_EVRCNW2K:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-NW2K: DELIVER_UL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure, 255 - eighth non critical)",
               *frameHeader );
        break;

      case ONEX_VOC_ID_EVRCWB:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-WB: DELIVER_UL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure)", *frameHeader );
        break;

      case ONEX_VOC_ID_QCELP13K:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: QCELP-13K: DELIVER_UL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure)", *frameHeader );
        break;
    }

    onex_buffer->vocoder_id = session_obj->vocoder_type;
    onex_voc_frame_info->frame_rate = frame_rate;
    onex_buffer->size = vs_buffer->frame_size - 1;
    mmstd_memcpy( onex_buffer->frame, onex_buffer->size, frameHeader + 1, onex_buffer->size );
    onex_buffer->flags = TRUE;

    cva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                           frameHeader, onex_buffer );

    break;
  }

  return rc;
}


static uint32_t cva_deliver_dl_vocoder_packet ( 
  cva_session_object_t* session_obj,
  onex_vocoder_buffer_t* onex_buffer
)
{
  uint32_t rc = APR_EOK;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_pktexg_buffer_t* vs_buffer = session_obj->vs_write_buf;
  uint8_t* frameHeader = NULL;
  uint8_t frame_rate;

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != session_obj->vocoder_type )
    {
      CVA_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    frameHeader = ( (uint8_t*)vs_buffer ) + sizeof(vs_pktexg_buffer_t);
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    rc = cva_map_voc_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate, &frame_rate );
    if ( rc || 
         // EVRC-A decoder does not support Quarter rate frames, hence drop it.
         ( ( ONEX_VOC_FRAME_RATE_QUARTER == onex_voc_frame_info->frame_rate) && 
           ( ONEX_VOC_ID_EVRCA == session_obj->vocoder_type ) )
       )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVA: Unsupported frame type[0x%08x]", onex_voc_frame_info->frame_rate );
      rc = APR_EFAILED;
      break;
    }

    switch ( session_obj->vocoder_type )
    {
      case ONEX_VOC_ID_EVRCA:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-A: GET_DL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 3-half, 4-full, 14-erasure)", frame_rate );
        break;

      case ONEX_VOC_ID_EVRCB:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-B: GET_DL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure)", frame_rate );
        break;

      case ONEX_VOC_ID_EVRCNW:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-NW: GET_DL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure, 255 - eighth non critical)", frame_rate );
        break;

      case ONEX_VOC_ID_EVRCNW2K:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-NW2K: GET_DL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure, 255 - eighth non critical)", frame_rate );
        break;

      case ONEX_VOC_ID_EVRCWB:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: EVRC-WB: GET_DL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure)", frame_rate );
        break;

      case ONEX_VOC_ID_QCELP13K:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "CVA: QCELP-13k: GET_DL_PACKET: FrameRate=(%d) (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure)", frame_rate );
        break;
    }

    vs_buffer->media_id = cva_map_vocoder_type_onex_to_vs( session_obj->vocoder_type ) ;
    *frameHeader = frame_rate;
    vs_buffer->frame_size = onex_buffer->size+1;
    mmstd_memcpy( frameHeader+1, onex_buffer->size, onex_buffer->frame, onex_buffer->size );

    cva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                           frameHeader, onex_buffer );

    break;
  }

  return rc;
}


/****************************************************************************
 * CVA ONEX SESSION ROUTINES                                                 *
 ****************************************************************************/

static uint32_t cva_onex_event_cb(
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
)
{
  uint32_t rc = APR_EOK;

  switch ( event_id )
  {
   case ONEX_VOC_EVENT_RQST_START:
   case ONEX_VOC_EVENT_RQST_STOP:
   case ONEX_VOC_EVENT_RQST_DTX_MODE:
   case ONEX_VOC_EVENT_RQST_MIN_MAX_RATE:
   case ONEX_VOC_EVENT_RQST_AVG_ENC_RATE:
    (void) cva_log_event_info( session_context, event_id );
     rc = cva_prepare_and_dispatch_cmd_packet( 
            session_context, event_id, params, size );
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "cva_onex_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static void cva_onex_vocoder_ul_cb (
 void* session_context,
 onex_vocoder_buffer_t* onex_buffer
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  session_obj = ( cva_session_object_t* )session_context;
  if( session_obj == NULL )
  {
    CVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return;
  }

  CVA_ACQUIRE_LOCK( session_obj->data_lock );

  onex_buffer->flags = FALSE;
  for( ;; )
  {
	
    if ( FALSE == session_obj->is_vs_ready )
    {
      rc = VS_EFAILED;
      break;
    }

    rc = cva_vs_get_encoder_buffer ( session_obj );
	
    if ( NULL == session_obj->vs_read_buf )
    {
      rc = VS_EFAILED;
      break;
    }

    ( void ) cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RUNNING );
    ( void ) cva_deliver_ul_vocoder_packet ( session_obj, onex_buffer );
    ( void ) cva_vs_return_encoder_buffer( session_obj );

    break;
  }

  if( FALSE == onex_buffer->flags )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "CVA: UL vocoder Packet not transfered to ONEX, is_vs_ready=(%d), "
           "vs_read_buf=(0x%08x)", session_obj->is_vs_ready, session_obj->vs_read_buf );
  }

  CVA_RELEASE_LOCK ( session_obj->data_lock );

  return;
}

static void cva_onex_vocoder_dl_cb (
  void* session_context,
  onex_vocoder_buffer_t* onex_buffer
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  session_obj = ( cva_session_object_t* )session_context;
  if( session_obj == NULL )
  {
    CVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return;
  }

  CVA_ACQUIRE_LOCK( session_obj->data_lock );

  for(;;)
  {
    if ( ( FALSE == session_obj->is_vs_ready ) || ( FALSE == onex_buffer->flags ) )
    {
      rc = VS_EFAILED;
      break;
    }

    rc = cva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      rc = VS_EFAILED;
      break;
    }

    rc = cva_deliver_dl_vocoder_packet( session_obj, onex_buffer );
    break;
  }

  if ( rc == APR_EOK )
  {
    flushCmd.handle = session_obj->vs_handle;
    flushCmd.enc_flush = FALSE; flushCmd.dec_flush = TRUE;
    rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
    CVA_REPORT_FATAL_ON_ERROR(rc);

    ( void ) cva_vs_put_decoder_buffer( session_obj );
  }
  else
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "CVA: DL vocoder buffer not tranfered to VS, is_vs_ready = (%d), "
           "onex_buffer->flags[%d], vs_write_buf=(0X%08X)",
           session_obj->is_vs_ready, onex_buffer->flags, session_obj->vs_write_buf );
  }

  CVA_RELEASE_LOCK ( session_obj->data_lock );

  return;
}

static uint32_t cva_onex_open_session (
 cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  onex_voc_cmd_open_t open_cmd;

  if ( NULL == session_obj )
  {
    CVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  open_cmd.ret_handle = &session_obj->onex_handle;
  open_cmd.asid = session_obj->asid;
  open_cmd.event_cb = cva_onex_event_cb;
  open_cmd.ul_cb = cva_onex_vocoder_ul_cb;
  open_cmd.dl_cb = cva_onex_vocoder_dl_cb;  
  open_cmd.session_context = ( void* ) session_obj;
  rc = onex_voice_call ( ONEX_VOC_OPEN_CMD, &open_cmd, sizeof( open_cmd ) );
  if ( ( APR_EOK != rc ) && ( APR_EUNSUPPORTED != rc ) ) 
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: Failed to open CDMA session for asid[%d]) with rc[0x%08x]",
           open_cmd.asid, rc );
  }
  else
  {
    rc = VS_EOK;
  }

  return rc;
}

static uint32_t cva_onex_close_session (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  onex_voc_cmd_close_t close_cmd;

  if ( NULL == session_obj )
  {
    CVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  close_cmd.handle = session_obj->onex_handle;
  rc = onex_voice_call ( ONEX_VOC_CLOSE_CMD, &close_cmd, sizeof( close_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: failed to close CDMA session for asid[%d] with rc[0x%08x]",
           session_obj->asid, rc );
  }
  else
  {
    session_obj->onex_handle = APR_NULL_V;
  }

  return rc;
}

static uint32_t cva_onex_start_session (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  onex_voc_cmd_start_t start_cmd;
 
  if ( NULL == session_obj )
  {
    CVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  start_cmd.handle = session_obj->onex_handle;
  rc = onex_voice_call ( ONEX_VOC_START_CMD, &start_cmd, sizeof( start_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: failed to start CDMA session for asid[%d] with rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

static uint32_t cva_onex_stop_session (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  onex_voc_cmd_start_t stop_cmd;
 
  if ( NULL == session_obj )
  {
    CVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  stop_cmd.handle = session_obj->onex_handle;
  rc = onex_voice_call ( ONEX_VOC_STOP_CMD, &stop_cmd, sizeof( stop_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "CVA: failed to stop CDMA session for asid[%d] with rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

/****************************************************************************
 * CVA CMDs/EVENTs HANDLING ROUTINES                                        *
 ****************************************************************************/

static uint32_t cva_process_vs_cmd_response_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  vs_common_event_cmd_response_t* evt_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_common_event_cmd_response_t, packet );
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "CVA: cmd response recieved for cmd_id[0x%08x], token[0x%08x], status[0x%08X]",
           evt_params->cmd_id, evt_params->token, evt_params->status );

    simple_obj = ( cva_simple_job_object_t* ) evt_params->token;
    if ( simple_obj == NULL ) break;

      simple_obj->is_completed = TRUE;
      simple_obj->status = evt_params->status;

    break;
  }

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t cva_process_vs_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = TRUE;
    cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_READY );

    CVA_RELEASE_LOCK ( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t cva_process_vs_not_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( session_obj->data_lock );

    if( APR_NULL_V != session_obj->vs_handle )
    {
      flushCmd.handle = session_obj->vs_handle;
      flushCmd.enc_flush = TRUE;
      flushCmd.dec_flush = TRUE;
      rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
      CVA_REPORT_FATAL_ON_ERROR(rc);
    }

    session_obj->is_vs_ready = FALSE;
    cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_NOT_READY );

    CVA_RELEASE_LOCK ( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t cva_process_onex_dtx_mode_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_dtx_mode_t* evt_params = NULL;

  for( ;; )
  {
    if( NULL == packet )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( onex_voc_event_rqst_dtx_mode_t, packet );
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK ( session_obj->data_lock );

    session_obj->dtx_mode = evt_params->enable_flag;
    ( void ) cva_vs_set_voc_dtx_mode( session_obj );

    CVA_RELEASE_LOCK ( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t cva_process_onex_min_max_rate_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_min_max_rate_t* evt_params = NULL;

  for( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( onex_voc_event_rqst_min_max_rate_t, packet );
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK ( session_obj->data_lock );

    session_obj->min_rate = evt_params->min_rate;
    session_obj->max_rate = evt_params->max_rate;
    ( void ) cva_vs_set_voc_min_max_rate ( session_obj );

    CVA_RELEASE_LOCK ( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t cva_process_onex_enc_rate_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_avg_enc_rate_t* evt_params = NULL;

  for( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( onex_voc_event_rqst_avg_enc_rate_t, packet );
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK ( session_obj->data_lock );

    session_obj->avg_enc_rate = evt_params->avg_enc_rate;
    ( void ) cva_vs_set_voc_enc_rate( session_obj );

    CVA_RELEASE_LOCK ( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( cva_apr_handle, packet );
  CVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t cva_process_onex_vocoder_start_event( 
  cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_start_t* evt_params = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( onex_voc_event_rqst_start_t, packet );
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "ONEX_VOC_EVENT_REQUEST_START(PROCESS) - ONEX configured with "
             "current_vocoder=(0x%08x), new_vocoder=(0x%08x)",
             session_obj->vocoder_type, evt_params->vocoder_id );

      if ( session_obj->is_onex_ready == TRUE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "cva_process_onex_vocoder_start_event() - ONEX already requested " );
        break;
      }

      if( cva_validate_cdma_vocoder_id( evt_params->vocoder_id ) == FALSE )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "cva_process_onex_vocoder_start_event() - Invalid vocoder Id",
               evt_params->vocoder_id );
        break;
      }

      /* Update the CVA call state. */
      session_obj->is_onex_ready = TRUE;
      session_obj->vocoder_type = evt_params->vocoder_id;

      if (session_obj->is_resource_granted == FALSE )
      {
        /* Request for voice resource. */
        requestResourceCmd.asid = session_obj->asid;
        requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_CDMA;
        rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                          sizeof(requestResourceCmd) );
        CVA_REPORT_FATAL_ON_ERROR(rc);

        cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
        break;
      }

      /* Create the Simple job object to track VS ENABLE. */
      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      /* Set cached vocoder properties. */
      rc = cva_vs_open_vocoder_session( session_obj );
      CVA_PANIC_ON_ERROR(rc);

      cva_vs_set_voc_media_type( session_obj );
      cva_vs_set_voc_dtx_mode ( session_obj );
      cva_vs_set_voc_enc_rate ( session_obj );
      cva_vs_set_voc_min_max_rate ( session_obj );
      cva_vs_set_voc_timing_offsets( session_obj );

      rc = cva_vs_enable_vocoder( session_obj, simple_obj );
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
      /* Free CMD/EVT packet memory. */
      ( void ) cva_mem_free_object ( ( cva_object_t* ) simple_obj );
      /* Send a START CMD notification to CDMA. */
      ( void ) cva_onex_start_session( session_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    CVA_RELEASE_LOCK ( session_obj->data_lock );
  }

  return rc;
}


static uint32_t cva_process_onex_vocoder_stop_event( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( cva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( rc );
      rc = APR_EUNEXPECTED;
      break;
    }

    CVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      if ( FALSE == session_obj->is_onex_ready )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "onex_vocoder_stop_event(PROCESS) - vocoder not started yet." );
        break;
      }

      /* Update the CVA call state. */
      session_obj->is_onex_ready = FALSE;
      session_obj->vocoder_type = CVA_VOCODER_ID_UNDEFINED_V;

      if ( TRUE == session_obj->is_resource_granted ) 
      {
        /* Create the Simple job object to track VS disable. */
          rc = cva_create_simple_job_object( session_obj->header.handle,
                 ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
          simple_obj = &ctrl->rootjob_obj->simple_job;

        rc = cva_vs_close_vocoder_session( session_obj, ( void* ) simple_obj );
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

    if ( APR_EOK == rc  )
    {
      /* Send voice resource released event to voice agent.
       * "REQUEST_STOP sent by ONEX without sending REQUEST_START" 
       * This can happen if ONEX called SELECT_OWNER during interRAT 
       * handover start. However the handover failed afterwards. So, ONEX 
       * did not call REQUEST_START, instead directly called REQUEST_STOP 
       * to indicate CVA that it no longer required vocoder. */
      session_obj->is_resource_granted = FALSE;
      releaseResourceCmd.asid = session_obj->asid;
      releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_CDMA;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                        sizeof(releaseResourceCmd) );
      CVA_REPORT_FATAL_ON_ERROR(rc);

      cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );

      /* Send stop notification to ONEX. */ 
      ( void ) cva_onex_stop_session ( session_obj );

      /* Free CMD/EVT packet memory. */
      ( void ) cva_mem_free_object ( ( cva_object_t* ) simple_obj );
    }

    break;
  }

  if ( session_obj != NULL )
  {
    CVA_RELEASE_LOCK ( session_obj->data_lock );
  }

  return rc;
}

static uint32_t cva_va_event_cb (
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
      rc = cva_prepare_and_dispatch_cmd_packet( NULL, event_id, params, size );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "CVA: Unsupported EventId=(%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t cva_process_resource_grant_cmd( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_event_resource_grant_t* event_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_grant_t, packet );
    if( event_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = cva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    CVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      session_obj->is_resource_granted = TRUE;
      session_obj->vsid = event_params->vsid;
      cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_GRANTED );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_GRANT recieved",
             session_obj->asid, session_obj->vsid );

      if ( session_obj->is_onex_ready == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "cva_process_resource_grant_cmd() - Traffic start request not "
             "available from ONEX" );
        break;
      }

      /* Create the Simple job object to track VS ENABLE. */
      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      /* Set cached vocoder properties. */
      rc = cva_vs_open_vocoder_session( session_obj );
      CVA_PANIC_ON_ERROR(rc);

      cva_vs_set_voc_media_type( session_obj );
      cva_vs_set_voc_dtx_mode ( session_obj );
      cva_vs_set_voc_enc_rate ( session_obj );
      cva_vs_set_voc_min_max_rate ( session_obj );
      cva_vs_set_voc_timing_offsets( session_obj );

      rc = cva_vs_enable_vocoder( session_obj, simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
        break;
      }

      /* Send a START CMD notification to CDMA. */
      ( void ) cva_onex_start_session( session_obj );
    }

    if ( rc == APR_EOK )
    {
      ( void ) cva_mem_free_object ( ( cva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
   CVA_RELEASE_LOCK ( session_obj->data_lock );
  }

  return rc;
}


static uint32_t cva_process_resource_revoke_cmd( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_event_resource_grant_t *event_params = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_grant_t, packet );
    if( event_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = cva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    CVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_REVOKE recieved",
             session_obj->asid, session_obj->vsid );

      /* Create the Simple job object to track VS_DISABLE. */
      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      rc = cva_vs_close_vocoder_session( session_obj, (void*) simple_obj );
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
      session_obj->is_resource_granted = FALSE;
      releaseResourceCmd.asid = session_obj->asid;
      releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_CDMA;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                        sizeof(releaseResourceCmd) );
      CVA_REPORT_FATAL_ON_ERROR(rc);

      cva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );

      ( void ) cva_mem_free_object ( ( cva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    CVA_RELEASE_LOCK ( session_obj->data_lock );
  }

  return rc;
}


/****************************************************************************
 * NONGATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/

static void cva_task_process_nongating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  cva_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;

  while( apr_list_remove_head( &cva_nongating_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;
    if ( packet == NULL )
    {
      CVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
    }

    /* Add back to vs_free_work_pkt_q. */
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &cva_free_work_pkt_q, &work_item->link );

    switch( packet->opcode )
    {
     /**
      * Handling routine for nongating work-items should take of release the
      * memory allocated for the CMD/EVENT packets.
      */
     case ONEX_VOC_EVENT_RQST_DTX_MODE:
       rc = cva_process_onex_dtx_mode_event( packet );
       break;

     case ONEX_VOC_EVENT_RQST_MIN_MAX_RATE:
       rc = cva_process_onex_min_max_rate_event( packet );
       break;

     case ONEX_VOC_EVENT_RQST_AVG_ENC_RATE:
       rc = cva_process_onex_enc_rate_event( packet );
       break;

     case VS_COMMON_EVENT_CMD_RESPONSE:
       rc = cva_process_vs_cmd_response_event( packet );
       break;

     case VS_VOC_EVENT_READY:
       rc = cva_process_vs_ready_event( packet );
       break;

     case VS_VOC_EVENT_NOT_READY:
       rc = cva_process_vs_not_ready_event( packet );
       break;

     default:
       /* Add remaining work items to the gating work queue. */
       rc = cva_queue_apr_packet ( packet, &cva_gating_work_pkt_q.cmd_q );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "CVA: NON-GATING: cmd pkt queuing failed rc[0x%08x]", rc );
         rc = __aprv2_cmd_free( cva_apr_handle, packet );
         CVA_REPORT_FATAL_ON_ERROR(rc);
       }
       break;
    }
  }

  return;
}

/****************************************************************************
 * GATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/
 
static void cva_task_process_gating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  cva_work_item_t* work_item;
  cva_gating_control_t* ctrl = &cva_gating_work_pkt_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
      case CVA_GATING_CMD_STATE_FETCH:
      {
        /* Fetch the next gating command to execute. */
        rc = apr_list_remove_head( &ctrl->cmd_q,
                                   ( ( apr_list_node_t** ) &work_item ) );

        if ( rc ) return;

        if ( work_item->packet == NULL )
        {
          CVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
        }

        ctrl->packet = work_item->packet;
        ctrl->state = CVA_GATING_CMD_STATE_EXECUTE;

        /* Add the vs_work_item_t back to vs_free_work_pkt_q. */
        work_item->packet = NULL;
        ( void ) apr_list_add_tail( &cva_free_work_pkt_q, &work_item->link );
      }
      break;

     case CVA_GATING_CMD_STATE_EXECUTE:
     case CVA_GATING_CMD_STATE_CONTINUE:
     {
       if ( CVA_GATING_CMD_STATE_EXECUTE == ctrl->state )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                "CVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) STARTED",
                ctrl->packet->opcode );
       }

       /* For Supported request_id, handler should take care of releasing 
        * memory allocated for packets. */
       switch ( ctrl->packet->opcode )
       {
        case ONEX_VOC_EVENT_RQST_START:
          rc = cva_process_onex_vocoder_start_event( ctrl );
          break;

        case ONEX_VOC_EVENT_RQST_STOP:
          rc = cva_process_onex_vocoder_stop_event( ctrl );
          break;

        case VAGENT_IVOICE_EVENT_RESOURCE_GRANT:
          rc = cva_process_resource_grant_cmd( ctrl );
          break;

        case VAGENT_IVOICE_EVENT_RESOURCE_REVOKE:
          rc = cva_process_resource_revoke_cmd( ctrl );
          break;

        default:
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "CVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) UNSUPPORTED",
                 ctrl->packet->opcode );
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
                  "CVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) ENDED",
                  ctrl->packet->opcode );
           rc = __aprv2_cmd_free( cva_apr_handle, ctrl->packet );
           CVA_REPORT_FATAL_ON_ERROR(rc)

           ctrl->packet = NULL;
           /* The current command is finished so fetch the next command. */
           ctrl->state = CVA_GATING_CMD_STATE_FETCH;
         }
         break;
       
         case APR_EPENDING:
           /**
            * Assuming the current pending command control routine returns
            * APR_EPENDING the overall progress stalls until one or more
            * external events or responses are received.
            */
           ctrl->state = CVA_GATING_CMD_STATE_CONTINUE;
           /**
            * Return from here so as to avoid unecessary looping till reponse
            * is recived.
            */
           return;

         default:
           CVA_PANIC_ON_ERROR( APR_EUNEXPECTED );
           break;
       }
     }
     break;

     default:
      CVA_PANIC_ON_ERROR( rc );
      break;
    }/* switch case ends. */
  }/* for loop ends. */

  return;
}

/****************************************************************************
 * CVA TASK ROUTINES                                                        *
 ****************************************************************************/

static int32_t cva_run ( void )
{
  cva_task_process_nongating_work_items( );
  cva_task_process_gating_work_items( );

  return APR_EOK;
}


static int32_t cva_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &cva_work_event );
  apr_event_signal( cva_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( cva_work_event );
    if ( rc ) break;

    cva_run( );
  }

  apr_event_destroy( cva_work_event );
  apr_event_signal( cva_control_event );

  return APR_EOK;
}

/****************************************************************************
 * CVA BOOT-UP and POWER-DOWN ROUTINES                                      *
 ****************************************************************************/

static uint32_t cva_gating_control_init (
  cva_gating_control_t* p_ctrl
)
{
  uint32_t rc = APR_EOK;

  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q,
                         cva_thread_lock_fn, cva_thread_unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  p_ctrl->state = CVA_GATING_CMD_STATE_FETCH;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return APR_EOK;
}  /* end of cva_gating_control_init () */

static uint32_t cva_gating_control_destroy (
  cva_gating_control_t* p_ctrl
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


static int32_t cva_init ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index;
  RCINIT_INFO info_handle = NULL;
  RCINIT_PRIO priority = 0;
  unsigned long stack_size = 0;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "====== cva_init(): ======" );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &cva_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &cva_thread_lock );
    apr_event_create( &cva_control_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &cva_heapmgr, ( ( void* ) &cva_heap_pool ),
                          sizeof( cva_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = cva_object_table;
    params.total_bits = CVA_HANDLE_TOTAL_BITS_V;
    params.index_bits = CVA_HANDLE_INDEX_BITS_V;
    params.lock_fn = cva_int_lock_fn;
    params.unlock_fn = cva_int_unlock_fn;
    rc = apr_objmgr_construct( &cva_objmgr, &params );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &cva_free_work_pkt_q, 
                           cva_int_lock_fn, cva_int_unlock_fn );
    for ( index = 0; index < CVA_NUM_WORK_PKTS_V; ++ index  )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &cva_work_pkts[index] );
      cva_work_pkts[index].packet = NULL;
      rc = apr_list_add_tail( &cva_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &cva_work_pkts[index] ) );
    }

    rc = apr_list_init_v2( &cva_nongating_work_pkt_q,
                           cva_int_lock_fn, cva_int_unlock_fn );
  }

  { /* Initialize gating work pkt queue. */
    rc = cva_gating_control_init( &cva_gating_work_pkt_q );
  }

  { /* Initialize the global session lock. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &cva_global_lock );
    CVA_PANIC_ON_ERROR( rc );
  }

  { /* Create the CVA task worker thread. */
#if ( !defined AVS_MPSS_TEST )
    info_handle = rcinit_lookup( CVA_TASK_NAME );
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
           "cva_init(): CVA task not registered with RCINIT" );
      priority = CVA_TASK_PRIORITY;
      stack_size = CVA_TASK_STACK_SIZE;
    }

    if ( ( priority > 255 ) || ( stack_size == 0 ) ) 
    {
      ERR_FATAL( "cva_init(): Invalid priority: %d or stack size: %d",
                 priority, stack_size, 0 );
    }

    rc = apr_thread_create( &cva_thread, CVA_TASK_NAME, TASK_PRIORITY(priority),
                            cva_task_stack, stack_size, cva_worker_thread_fn , NULL );
    CVA_PANIC_ON_ERROR( rc );

    apr_event_wait( cva_control_event );
  }
  cva_is_initialized = TRUE;

  return rc;
}

static int32_t cva_postinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  vagent_ivoice_cmd_register_t registerCmd;

  rc = __aprv2_cmd_register2(
         &cva_apr_handle, cva_my_dns, sizeof( cva_my_dns ), 0,
         cva_isr_dispatch_fn, NULL, &cva_my_addr );
  CVA_PANIC_ON_ERROR(rc);

  /* Initialize the mapping info and open onex voice instance. */
  for( index = 0; index < CVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* Create and initialize CVA session object. */
    rc =  cva_create_session_object ( &cva_session_obj_list[index] );
    CVA_PANIC_ON_ERROR( rc );

    /* Open CDMA voice session instance. */
    cva_session_obj_list[index]->asid = ( sys_modem_as_id_e_type ) index;
    rc = cva_onex_open_session( cva_session_obj_list[index] );
    CVA_PANIC_ON_ERROR (rc);
  }

  /* Registring with voice agent for resource. */
  registerCmd.client_id = VAGENT_VOICE_CLIENT_CDMA;
  registerCmd.client_cb = cva_va_event_cb;
  registerCmd.client_data = NULL;
  rc = vagent_call( VAGENT_IVOICE_CMD_REGISTER, (void*)&registerCmd, sizeof(registerCmd) );
  CVA_PANIC_ON_ERROR (rc);

  return rc;
}


static int32_t cva_predeinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index = 0;

  /* Close onex Session instance. */
  for( index = 0; index < CVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* CVA CLOSE's CDMA session instance for all available ASID. */
    ( void ) cva_onex_close_session( cva_session_obj_list[index] );

    /* Free CVA session object for all VSID. */
    ( void ) apr_lock_destroy( cva_session_obj_list[index]->data_lock );
    ( void ) cva_mem_free_object( (cva_object_t*)cva_session_obj_list[ index ] );
  }

  return rc;
}


static int32_t cva_deinit ( void )
{
  uint32_t rc = APR_EOK;

  cva_is_initialized = FALSE;

  apr_event_signal_abortall( cva_work_event );
  apr_event_wait( cva_control_event );

  /* Release gating control structures */
  ( void ) cva_gating_control_destroy( &cva_gating_work_pkt_q );

  /* Release work queue */
  ( void ) apr_list_destroy( &cva_free_work_pkt_q );
  ( void ) apr_list_destroy( &cva_nongating_work_pkt_q );

  /* Deinitialize the object handle table. */
  apr_objmgr_destruct( &cva_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  ( void ) apr_event_destroy( cva_control_event );
  ( void ) apr_lock_destroy( cva_int_lock );
  ( void ) apr_lock_destroy( cva_thread_lock );
  ( void ) apr_lock_destroy( cva_global_lock );

  return rc;
}


CVA_EXTERNAL uint32_t cva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;

  if ( ( FALSE == cva_is_initialized ) && ( DRV_CMDID_INIT != cmd_id ) )
  {
    CVA_REPORT_FATAL_ON_ERROR( APR_ENOSERVICE );
    return APR_ENOSERVICE;
  }

  switch ( cmd_id )
  {
  case DRV_CMDID_INIT:
    rc = cva_init( );
    break;

  case DRV_CMDID_POSTINIT:
    rc = cva_postinit( );
    break;

  case DRV_CMDID_PREDEINIT:
    rc = cva_predeinit( );
    break;

  case DRV_CMDID_DEINIT:
    rc = cva_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

