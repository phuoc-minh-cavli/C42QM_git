/*
   Copyright (C) 2015-2019 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/src/gva_module.c#1 $
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

/* GSM APIs. */
#include "gsm_voice_server_if.h"

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
#include "gva_if.h"
#include "gva_i.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Currently voice agent support max two subscription and max two VSID. */
#define GVA_MAX_NUM_OF_SESSIONS_V ( 2 )

#define GVA_ENC_PKT_MISS_THRESHOLD ( 2 )


/* ENC/DEC Offset definitions.
 *
 * Uplink/Downlink callback are not 20msec periodic for GSM.
 *
 * Uplink packets are picked as early as 12700 from VFR, hence encoder offset
 * is 10000 with 2.7msec cushion.
 * 
 * Downlink packets are delivered as late as 1000 from VFR, hence decoder request
 * offset is 3300 with 2.3msec cushion.
 */
#define GVA_ENC_OFFSET_US ( 10000 )
#define GVA_DEC_REQ_OFFSET_US ( 3300 )
#define GVA_DEC_OFFSET_US ( 4300 )

/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

/* GVA address is set at initialization. */
char_t gva_my_dns[] = "qcom.audio.gva";
static uint16_t gva_my_addr;
static uint32_t gva_apr_handle;

static apr_lock_t gva_int_lock;
static apr_lock_t gva_thread_lock;
static apr_event_t gva_control_event;

static apr_memmgr_type gva_heapmgr;
static uint8_t gva_heap_pool[ GVA_HEAP_SIZE_V ];

static apr_objmgr_t gva_objmgr;
static apr_objmgr_object_t gva_object_table[ GVA_MAX_OBJECTS_V ];

static gva_gating_control_t gva_gating_work_pkt_q;
static apr_list_t gva_nongating_work_pkt_q;
static apr_list_t gva_free_work_pkt_q;
static gva_work_item_t gva_work_pkts[ GVA_NUM_WORK_PKTS_V ];

static apr_event_t gva_work_event;
static apr_thread_t gva_thread;
static uint8_t gva_task_stack[ GVA_TASK_STACK_SIZE ];

static gva_session_object_t* gva_session_obj_list[GVA_MAX_NUM_OF_SESSIONS_V];
static apr_lock_t gva_global_lock;

static bool_t gva_is_eamr_enabled = FALSE;
static bool_t gva_is_initialized = FALSE; 

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void gva_int_lock_fn ( void )
{
  apr_lock_enter( gva_int_lock );
}

static void gva_int_unlock_fn ( void )
{
  apr_lock_leave( gva_int_lock );
}

static void gva_thread_lock_fn ( void )
{
  apr_lock_enter( gva_thread_lock );
}

static void gva_thread_unlock_fn ( void )
{
  apr_lock_leave( gva_thread_lock );
}

static void gva_signal_run ( void )
{
  apr_event_signal( gva_work_event );
}

/****************************************************************************
 * GVA CMDs & EVENTs PACKET QUEUING FUNCTIONS                               *
 ****************************************************************************/

/**
 * Queues the aprv2_packet_t (queues both incoming and pending packets to 
 * corresponding queues). In case of failure to queue a apr packet, packet 
 * will be automatically freed.
 */
static uint32_t gva_queue_apr_packet (
  aprv2_packet_t* packet,
  apr_list_t* cmd_q
)
{
  uint32_t rc = VS_EOK;
  gva_work_item_t* work_item = NULL;
  
  if( packet == NULL )
  {
    GVA_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    return VS_EFAILED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &gva_free_work_pkt_q, (apr_list_node_t**)&work_item );
    if ( NULL == work_item )
    {
      GVA_REPORT_FATAL_ON_ERROR(VS_ENORESOURCE);
      break;
    }

    work_item->packet = packet;
    rc = apr_list_add_tail( cmd_q, &work_item->link );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: error in queuing APR packet: opcode[0x%08x], rc[0x%08x]", packet->opcode, rc );
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &gva_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      gva_signal_run( );
    }

    break;
  } /* for loop ends. */

  return rc;
}


static int32_t gva_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t rc;
  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}


/****************************************************************************
 * GVA CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                  *
 ****************************************************************************/

static uint32_t gva_prepare_and_dispatch_cmd_packet (
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
    rc = __aprv2_cmd_alloc_ext( gva_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                APR_NULL_V, APR_NULL_V,
                                APR_NULL_V, APR_NULL_V, 
                                (uint32_t)session_context, cmd_id,
                                params_size, &packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: APR packet allocation failed with rc=[0x%08x]", rc );
      break;
    }

    if ( ( params_size > 0 ) && ( cmd_params != NULL ) )
    {
      payload = APRV2_PKT_GET_PAYLOAD( void, packet);
      mmstd_memcpy( payload, params_size, cmd_params, params_size );
    }

    /* Queue the command packet for processing. */
    rc = gva_queue_apr_packet( packet, &gva_nongating_work_pkt_q );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: APR packet queuing failed. rc[0x%08x]", rc );
      rc = __aprv2_cmd_free( gva_apr_handle, packet );
      GVA_REPORT_FATAL_ON_ERROR(rc);
    }

    break;
  }

  return rc;
}


/****************************************************************************
 * GVA OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES             *
 ****************************************************************************/
static uint32_t gva_mem_alloc_object (
  uint32_t size,
  gva_object_t** ret_object
)
{
  int32_t rc;
  gva_object_t* gva_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the GVA object. */
    gva_obj = apr_memmgr_malloc( &gva_heapmgr, size );
    if ( gva_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "gva_mem_alloc_object(): Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the MVS object. */
    rc = apr_objmgr_alloc_object( &gva_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "gva_mem_alloc_object(): Out of objects, rc = (0x%08X)", rc );
      apr_memmgr_free( &gva_heapmgr, gva_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = gva_obj;

    /* Initialize the base MVS object header. */
    gva_obj->header.handle = objmgr_obj->handle;
    gva_obj->header.type = GVA_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = gva_obj;

  return APR_EOK;
}

static uint32_t gva_mem_free_object (
  gva_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &gva_objmgr, object->header.handle );
  apr_memmgr_free( &gva_heapmgr, object );

  return APR_EOK;
}

static uint32_t gva_create_session_object ( 
  gva_session_object_t** ret_session_obj )
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = gva_mem_alloc_object( sizeof( gva_session_object_t ),
                             ( ( gva_object_t** ) &session_obj ) );
  if ( rc )
  {
    *ret_session_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    session_obj->header.type = GVA_OBJECT_TYPE_ENUM_SESSION;

    session_obj->vsid = GVA_VSID_UNDEFINED_V;
    session_obj->vocoder_state = VOICELOG_IEVENT_VOCODER_STATE_RELEASED;

    /* Initialzing the miss count with the threshold so that GVA can push the homing sequence
       straight away during call start and handover scenarios. Miss count will be reset to zero when the
       first encoder packet arrives, so the threshold will come into play only when there are
       packet misses during steady state.
     */
    session_obj->enc_packet_miss_count = GVA_ENC_PKT_MISS_THRESHOLD;
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->data_lock );
    GVA_PANIC_ON_ERROR(rc);


    session_obj->is_resource_granted = FALSE;
    session_obj->vocoder_type = GVA_VOCODER_ID_UNDEFINED_V;
    session_obj->codec_mode = GVA_CODEC_MODE_UNDEFINED;
    session_obj->dtx_mode = FALSE;

    session_obj->vs_handle = APR_NULL_V;
    session_obj->vs_read_buf = NULL;
    session_obj->vs_write_buf = NULL;
    session_obj->is_vs_ready = FALSE;
  }

  *ret_session_obj = session_obj;

  return APR_EOK;
}

static uint32_t gva_create_simple_job_object (
  uint32_t parentjob_handle,
  gva_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  gva_simple_job_object_t* gva_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = gva_mem_alloc_object( sizeof( gva_simple_job_object_t ),
                             ( ( gva_object_t** ) &gva_obj ) );
  if ( rc )
  {
    *ret_job_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    gva_obj->header.type = GVA_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    gva_obj->context_handle = parentjob_handle;
    gva_obj->is_completed = 0;
  }

  *ret_job_obj = gva_obj;

  return APR_EOK;
}


/****************************************************************************
 * GVA GSM <> VS MAPPING  ROUTINES                                          *
 ****************************************************************************/

static uint32_t gva_map_vocamr_codec_mode_gsm_to_vs( 
  uint32_t gsm_codec_mode,
  uint32_t* vs_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( gsm_codec_mode )
  {
   case GSM_IVOCAMR_CODEC_MODE_0475:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0475;
     break;

   case GSM_IVOCAMR_CODEC_MODE_0515:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0515;
     break;

   case GSM_IVOCAMR_CODEC_MODE_0590:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0590;
     break;

   case GSM_IVOCAMR_CODEC_MODE_0670:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0670;
     break;

   case GSM_IVOCAMR_CODEC_MODE_0740:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0740;
     break;

   case GSM_IVOCAMR_CODEC_MODE_0795:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0795;
     break;

   case GSM_IVOCAMR_CODEC_MODE_1020:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_1020;
     break;

   case GSM_IVOCAMR_CODEC_MODE_1220:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_1220;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_codec_mode = 0xFFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocamr_codec_mode_vs_to_gsm( 
  uint32_t vs_codec_mode,
  uint32_t* gsm_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( vs_codec_mode )
  {
   case VS_VOCAMR_CODEC_MODE_0475:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_0475;
     break;

   case VS_VOCAMR_CODEC_MODE_0515:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_0515;
     break;

   case VS_VOCAMR_CODEC_MODE_0590:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_0590;
     break;

   case VS_VOCAMR_CODEC_MODE_0670:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_0670;
     break;

   case VS_VOCAMR_CODEC_MODE_0740:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_0740;
     break;

   case VS_VOCAMR_CODEC_MODE_0795:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_0795;
     break;

   case VS_VOCAMR_CODEC_MODE_1020:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_1020;
     break;

   case VS_VOCAMR_CODEC_MODE_1220:
     *gsm_codec_mode = GSM_IVOCAMR_CODEC_MODE_1220;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *gsm_codec_mode = 0xFFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocamr_frame_type_vs_to_gsm(
  uint32_t vs_frame_type,
  uint32_t* gsm_frame_type
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_type )
  {
   case VS_VOCAMR_FRAME_TYPE_NO_DATA:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_NO_DATA;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_SID_BAD:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SID_BAD;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_SID_UPDATE:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SID_UPDATE;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_SID_FIRST:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SID_FIRST;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_SPEECH_BAD:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SPEECH_BAD;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_ONSET:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SPEECH_ONSET;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SPEECH_DEGRADED;
     break;
  
   case VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD:
     *gsm_frame_type = GSM_IVOCAMR_FRAME_TYPE_SPEECH_GOOD;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *gsm_frame_type = 0xFFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocamr_frame_type_gsm_to_vs(
  uint32_t gsm_frame_type,
  uint32_t* vs_frame_type
)
{
  uint32_t rc = APR_EOK;

  switch( gsm_frame_type )
  {
   case GSM_IVOCAMR_FRAME_TYPE_NO_DATA:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_NO_DATA;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SID_BAD:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_SID_BAD;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SID_UPDATE:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_SID_UPDATE;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SID_FIRST:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_SID_FIRST;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SPEECH_BAD:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_BAD;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SPEECH_ONSET:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_ONSET;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SPEECH_DEGRADED:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED;
     break;
  
   case GSM_IVOCAMR_FRAME_TYPE_SPEECH_GOOD:
     *vs_frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_type = 0xFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocamrwb_codec_mode_gsm_to_vs( 
  uint32_t gsm_codec_mode,
  uint32_t* vs_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( gsm_codec_mode )
  {
   case GSM_IVOCAMRWB_CODEC_MODE_0660:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_0660;
     break;
  
   case GSM_IVOCAMRWB_CODEC_MODE_0885:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_0885;
     break;
  
   case GSM_IVOCAMRWB_CODEC_MODE_1265:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1265;
     break;
  
   case GSM_IVOCAMRWB_CODEC_MODE_1425:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1425;
     break;
    
   case GSM_IVOCAMRWB_CODEC_MODE_1585:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1585;
     break;
    
   case GSM_IVOCAMRWB_CODEC_MODE_1825:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1825;
     break;
    
   case GSM_IVOCAMRWB_CODEC_MODE_1985:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1985;
     break;
   
   case GSM_IVOCAMRWB_CODEC_MODE_2305:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_2305;
     break;
  
   case GSM_IVOCAMRWB_CODEC_MODE_2385:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_2385;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_codec_mode = 0xFFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocamrwb_codec_mode_vs_to_gsm( 
  uint32_t vs_codec_mode,
  uint32_t* gsm_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( vs_codec_mode )
  {
   case VS_VOCAMRWB_CODEC_MODE_0660:
     *gsm_codec_mode =GSM_IVOCAMRWB_CODEC_MODE_0660;
     break;
  
   case VS_VOCAMRWB_CODEC_MODE_0885:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_0885;
     break;
  
   case VS_VOCAMRWB_CODEC_MODE_1265:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_1265;
     break;
  
   case VS_VOCAMRWB_CODEC_MODE_1425:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_1425;
     break;
    
   case VS_VOCAMRWB_CODEC_MODE_1585:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_1585;
     break;
    
   case VS_VOCAMRWB_CODEC_MODE_1825:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_1825;
     break;
    
   case VS_VOCAMRWB_CODEC_MODE_1985:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_1985;
     break;
   
   case VS_VOCAMRWB_CODEC_MODE_2305:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_2305;
     break;
  
   case VS_VOCAMRWB_CODEC_MODE_2385:
     *gsm_codec_mode = GSM_IVOCAMRWB_CODEC_MODE_2385;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *gsm_codec_mode = 0xFFFFFFFF;
     break;
  }

  return rc;
}


static uint32_t gva_map_vocamrwb_frame_type_vs_to_gsm(
  uint32_t vs_frame_type,
  uint32_t* gsm_frame_type
)
{
  uint32_t rc = APR_EOK;
  
  switch( vs_frame_type )
  {
   case VS_VOCAMRWB_FRAME_TYPE_NO_DATA:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_NO_DATA;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SID_BAD:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SID_BAD;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SID_UPDATE;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SID_FIRST:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SID_FIRST;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_BAD;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_LOST;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_DEGRADED;
     break;
  
   case VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD:
     *gsm_frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *gsm_frame_type = 0xFFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocamrwb_frame_type_gsm_to_vs(
  uint32_t gsm_frame_type,
  uint32_t* vs_frame_type
)
{
  uint32_t rc = APR_EOK;
  
  switch( gsm_frame_type )
  {
   case GSM_IVOCAMRWB_FRAME_TYPE_NO_DATA:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_NO_DATA;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SID_BAD:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SID_BAD;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SID_UPDATE:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SID_FIRST:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SID_FIRST;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_BAD:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_LOST:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_DEGRADED:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED;
     break;
  
   case GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_GOOD:
     *vs_frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_type = 0xFFFFFFFF;
     break;
  }

  return rc;
}

static uint32_t gva_map_vocoder_type_gsm_to_vs(
  uint32_t gsm_vocoder_type
)
{
  uint32_t vs_media_id = 0xFFFFFFFF;

  switch ( gsm_vocoder_type )
  {
   case GSM_IVOCODER_ID_AMR:
     vs_media_id = (TRUE== gva_is_eamr_enabled)?VS_VOCEAMR_MEDIA_ID : VS_VOCAMR_MEDIA_ID;
     break;

   case GSM_IVOCODER_ID_AMRWB:
     vs_media_id = VS_VOCAMRWB_MEDIA_ID;
     break;

   case GSM_IVOCODER_ID_EFR:
    vs_media_id = VS_VOCEFR_MEDIA_ID;
    break;

   case GSM_IVOCODER_ID_FR:
    vs_media_id = VS_VOCFR_MEDIA_ID;
    break;

   case GSM_IVOCODER_ID_HR:
     vs_media_id = VS_VOCHR_MEDIA_ID;
     break;
  }

  return vs_media_id;
}

static bool_t gva_validate_gsm_vocoder_id (
  uint32_t vocoder_id
)
{
  bool_t rc = FALSE;

  switch ( vocoder_id )
  {
   case GSM_IVOCODER_ID_AMR:
   case GSM_IVOCODER_ID_AMRWB:
   case GSM_IVOCODER_ID_EFR:
   case GSM_IVOCODER_ID_FR:
   case GSM_IVOCODER_ID_HR:
     rc = TRUE;
     break;

   default:
     break;
  }

  return rc;
}

/****************************************************************************
 * GVA COMMON ROUTINES                                                      *
 ****************************************************************************/

static void gva_publish_vocoder_state (
  gva_session_object_t* session_obj,
  uint32_t vocoder_state
)
{
  if( ( NULL != session_obj ) &&
      ( vocoder_state != session_obj->vocoder_state ) )
  {
    /* Publish vocoder state for voice call on a subscription. */
   session_obj->vocoder_state = vocoder_state;
   voicelog_event_report( session_obj->asid, session_obj->vsid,
                          session_obj->vocoder_state, VOICELOG_ICOMMON_NETWORK_ID_GSM,
                          gva_map_vocoder_type_gsm_to_vs( session_obj->vocoder_type ) );
  }

  return;
}


static uint32_t gva_log_vocoder_frame ( 
  gva_session_object_t* session_obj,
  uint32_t log_code,
  uint8_t* frameHeader,
  gsm_ivocoder_buffer_t* gsm_buffer
)
{
  voicelog_ipacket_cmd_commit_data_t log_cmd_param; 

  log_cmd_param.version = VOICELOG_IPACKET_VOCODER_DATA_VERSION_V;
  log_cmd_param.log_code = log_code;
  log_cmd_param.vsid = session_obj->vsid;
  log_cmd_param.network_id = VOICELOG_ICOMMON_NETWORK_ID_GSM;
  log_cmd_param.timestamp = 0;
  log_cmd_param.tap_point_id = VOICELOG_IPACKET_TAP_POINT_ID_VOICE_ADAPTER;

  log_cmd_param.media_id = gva_map_vocoder_type_gsm_to_vs ( gsm_buffer->vocoder_id );
  log_cmd_param.frame_header = *frameHeader;
  log_cmd_param.frame = gsm_buffer->frame;
  log_cmd_param.frame_size = gsm_buffer->size;
  log_cmd_param.data_extension = NULL;

  /* Log data. */
  ( void ) voicelog_call ( VOICELOG_IPACKET_CMD_COMMIT_DATA, (void*)&log_cmd_param, 
                           sizeof( voicelog_ipacket_cmd_commit_data_t ) );

  return APR_EOK;
}


/****************************************************************************
 * GVA VS SESSION ROUTINES                                                  *
 ****************************************************************************/

static void gva_log_event_info(
  void* session_context,
  uint32_t event_id
)
{
  gva_session_object_t* session_obj = ( gva_session_object_t* ) session_context;

  if ( session_context == NULL ) return;

  switch( event_id )
  {
    case VS_VOC_EVENT_NOT_READY:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "GVA: VSID=(0x%08x): VS_VOC_EVENT_NOT_READY recieved",
             session_obj->vsid );
    }
    break;

    case VS_VOC_EVENT_READY:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "GVA: VSID=(0x%08x): VS_VOC_EVENT_READY recieved",
             session_obj->vsid );
    }
    break;
 
    case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "GVA: VSID=(0x%08x): VS_VOC_EVENT_OPERATING_MODE_UPDATE recieved",
             session_obj->vsid );
    }
    break;
 
    case GSM_IVOICE_EVENT_START_REQUEST:
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "GVA: ASID=(%d), VSID=(0x%08x): GSM_IVOICE_EVENT_REQUEST_START recieved",
             session_obj->asid, session_obj->vsid );
    }
    break;
 
    case GSM_IVOICE_EVENT_STOP_REQUEST:
     {
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "GVA: ASID=(%d), VSID=(0x%08x): GSM_IVOICE_EVENT_STOP_REQUEST recieved",
              session_obj->asid, session_obj->vsid );
     }
     break;
 
     case GSM_IVOICE_EVENT_SELECT_OWNER:
     {
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "GVA: ASID=(%d), VSID=(0x%08x): GSM_IVOICE_EVENT_SELECT_OWNER recieved",
              session_obj->asid, session_obj->vsid );
     }
     break;
 
     case GSM_IVOICE_EVENT_REQUEST_CODEC_MODE:
     {
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "GVA: ASID=(%d), VSID=(0x%08x): GSM_IVOICE_EVENT_REQUEST_CODEC_MODE recieved",
              session_obj->asid, session_obj->vsid );
     }
     break;
 
     case GSM_IVOICE_EVENT_REQUEST_DTX_MODE:
     {
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "GVA: ASID=(%d), VSID=(0x%08x): GSM_IVOICE_EVENT_REQUEST_DTX_MODE recieved",
              session_obj->asid, session_obj->vsid );
     }
     break;

     case VS_COMMON_EVENT_CMD_RESPONSE:
     default:
       break;
  }

  return;
}

static uint32_t gva_vs_event_cb(
 uint32_t event_id,
 void* params,
 uint32_t size,
 void* session_context
)
{
  uint32_t rc = APR_EOK;

  if ( gva_is_initialized == FALSE ) return APR_EOK;

  switch ( event_id )
  {
    case VS_COMMON_EVENT_CMD_RESPONSE:
    case VS_VOC_EVENT_NOT_READY:
    case VS_VOC_EVENT_READY:
    case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
    {
      (void) gva_log_event_info( ( gva_session_object_t* ) session_context, event_id );
      rc = gva_prepare_and_dispatch_cmd_packet( session_context, event_id, params, size );
    }
    break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "gva_vs_event_cb(): Unsupported event (%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}


static void gva_vs_set_voc_codec_mode (
  gva_session_object_t* session_obj
)
{

  uint32_t rc = APR_EOK;
  vs_param_enc_rate_t param;
  vs_common_cmd_set_param_t setParamCmd;
  uint32_t codec_mode;

  if ( APR_NULL_V == session_obj->vs_handle ) return;

  switch ( session_obj->vocoder_type )
  {
    case GSM_IVOCODER_ID_AMR:
    {
      rc = gva_map_vocamr_codec_mode_gsm_to_vs ( session_obj->codec_mode, &codec_mode );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR: Unsupported CodecMode=(0x%08x)", session_obj->codec_mode );
      }
      else
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "GVA: AMR: CodecMode=(%d) sent to VS", codec_mode );
      }
    }
    break;

    case GSM_IVOCODER_ID_AMRWB:
    {
      rc = gva_map_vocamrwb_codec_mode_gsm_to_vs ( session_obj->codec_mode, &codec_mode );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMRWB: Unsupported CodecMode=(0x%08x)", session_obj->codec_mode );
      }
      else
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "GVA: AMRWB: CodecMode=(%d) sent to VS", codec_mode );
      }
    }
    break;

    case GSM_IVOCODER_ID_EFR:
    case GSM_IVOCODER_ID_FR:
    case GSM_IVOCODER_ID_HR:
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "gva_set_voc_codec_mode(): Unsupported Vocoder=(0x%08x)",
             session_obj->vocoder_type );
      return;
  }

  param.rate = (uint8_t) codec_mode;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_RATE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: failed to set codec_mode=(%d), vocoder=(0x%08x)",
           session_obj->codec_mode, session_obj->vocoder_type );
  }

  return;
}


static void gva_vs_set_voc_dtx_mode (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_dtx_v2_t param;
  vs_common_cmd_set_param_t setParamCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) return;

  switch ( session_obj->vocoder_type )
  {
   case GSM_IVOCODER_ID_AMR:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "GVA: AMR dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
     break;

   case GSM_IVOCODER_ID_AMRWB:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "GVA: AMRWB dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    break;

   case GSM_IVOCODER_ID_EFR:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "GVA: EFR dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    break;

   case GSM_IVOCODER_ID_FR:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "GVA: FR dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    break;

   case GSM_IVOCODER_ID_HR:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "GVA: HR dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "gva_set_voc_dtx_mode(): Unsupported vocoder=(0x%08x)",
           session_obj->vocoder_type );
     return;
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
           "GVA: failed to set dtx_mode=(%d), vocoder=(0x%08x)",
           session_obj->dtx_mode, session_obj->vocoder_type );
  }

  return;
}

static void gva_vs_set_voc_media_type (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_media_id_t param;
  vs_common_cmd_set_param_t setParamCmd;

  param.media_id = gva_map_vocoder_type_gsm_to_vs( session_obj->vocoder_type );
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_VOC_MEDIA_ID;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: failed to set mediaID[0x%08x]", session_obj->vocoder_type );
  }

  return;
}


static void gva_vs_set_voc_timing_offsets (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_timing_offsets_t timingParam;
  vs_common_cmd_set_param_t setParamCmd;

  timingParam.enc_offset = GVA_ENC_OFFSET_US;
  timingParam.dec_req_offset = GVA_DEC_REQ_OFFSET_US;
  timingParam.dec_offset = GVA_DEC_OFFSET_US;
  timingParam.vfr_mode = 1;

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&timingParam;
  setParamCmd.param_id = VS_PARAM_VOC_TIMING_OFFSETS;
  setParamCmd.size = sizeof( timingParam );
  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "GVA: failed to set timing Offsets" );
  }

  return;
}

static uint32_t gva_vs_return_encoder_buffer (
  gva_session_object_t* session_obj
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
           "GVA: Failed to return read buffer, buffer[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  session_obj->vs_read_buf = NULL;
  return rc;
}

static uint32_t gva_vs_get_encoder_buffer (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = VS_ENOTEXIST;
  vs_pktexg_buffer_t* tempReadBuf = NULL;
  vs_pktexg_cmd_get_encoder_buffer_t getEncBufCmd;

  /* Read B2B to get latest encoder buffer and return previous encoder 
   * buffer successively. */
  while( TRUE )
  {
    if ( APR_NULL_V == session_obj->vs_handle ) break;

    getEncBufCmd.handle = session_obj->vs_handle;
    getEncBufCmd.buffer = (void*)&tempReadBuf;

    rc = vs_call( VS_PKTEXG_CMD_GET_ENCODER_BUFFER, (void*)&getEncBufCmd, sizeof( getEncBufCmd ) );
    if ( rc || ( NULL == tempReadBuf ) ) break;

    /* Return previous encoder buffer as we have latest buffer available. */
    if( NULL != session_obj->vs_read_buf )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: returning previous encoder buffer, GL1 missed an encoder callback " );
      ( void ) gva_vs_return_encoder_buffer( session_obj );
    }

    session_obj->vs_read_buf = tempReadBuf;
    tempReadBuf = NULL;
  }

  if( NULL == session_obj->vs_read_buf )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: Failed to get read encoder buffer, rc[0x%08x]", rc );
  }

  return rc;
}


static uint32_t gva_vs_get_decoder_buffer (
  gva_session_object_t* session_obj
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
           "GVA: Failed to get vs_write_buf,  rc[0x%08x]", rc );
  }

  return rc;
}


static uint32_t gva_vs_put_decoder_buffer (
  gva_session_object_t* session_obj
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
           "GVA: Failed to return vs_write_buf[0x%08x], rc[0x%08x]", session_obj->vs_write_buf, rc );
  }

  session_obj->vs_write_buf = NULL;
  return APR_EOK;
}


static uint32_t gva_vs_open_vocoder_session (
 gva_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;

  /* Open VS session. */
  open_cmd.ret_handle = &session_obj->vs_handle;
  open_cmd.vsid = session_obj->vsid;
  open_cmd.network_id = VS_COMMON_NETWORK_ID_GSM;
  open_cmd.session_context = ( void* )session_obj;
  open_cmd.event_cb = gva_vs_event_cb;

  rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: failed to open VS session rc = (0x%08x)", rc );
  }

  return rc;
}


static uint32_t gva_vs_enable_vocoder (
  gva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_enable_t enableCmd;

  /* Update Vocoder Session Number for vocoder logging purpose. */
  ( void ) voicelog_session_update ( session_obj->vsid,
                                     VOICELOG_INFO_ENUM_VOCODER_SESSION_NUMBER);

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "GVA: Enabling VS!!!" );

  enableCmd.handle = session_obj->vs_handle;
  enableCmd.token = token;
  rc = vs_call( VS_VOC_CMD_ENABLE, &enableCmd, sizeof( enableCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: Failed to enable VS session, handle[0x%08x], rc[0x%08x]",
           session_obj->vs_handle, rc );
  }
  else
  {
    rc = APR_EPENDING;
  }

  return rc;
}


static uint32_t gva_vs_close_vocoder_session (
  gva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_close_t closeCmd;

  /* Check to avoid sending redundant CLOSE command. */
  if ( session_obj->vs_handle == APR_NULL_V ) return VS_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "GVA: Closing VS!!!" );

  closeCmd.handle = session_obj->vs_handle;
  closeCmd.token = token;
  rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&closeCmd, sizeof( closeCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: Failed to close VS session, handle[0x%08x], rc[0x%08x]",
           session_obj->vs_handle, rc );
  }
  else
  {
    session_obj->vs_handle = APR_NULL_V;
    rc = APR_EPENDING;
  }

  return rc;
}

static uint32_t gva_deliver_ul_packet_amr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivocamr_frame_info_t* gsm_frame_info = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  vs_vocamr_frame_header_t* frameHeader = NULL;
  uint8_t* frame = NULL;
  uint32_t frame_type;
  uint32_t codec_mode = 0;

  for( ;; )
  {
    if( NULL == session_obj->vs_read_buf )
    {
      /* Send silence frame on uplink if number of encoder missed crosses a 
       * threshold. */
      session_obj->enc_packet_miss_count++;
      if( session_obj->enc_packet_miss_count <= GVA_ENC_PKT_MISS_THRESHOLD ) break;

      rc = gva_map_vocamr_codec_mode_gsm_to_vs ( session_obj->codec_mode, &codec_mode );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: (E)AMR: DELIVER_UL_PACKET: Unsupported codec mode =(%d)",
               session_obj->codec_mode );
        break;
      }

      vs_vocamr_frame_header_t frameHeader;
      frameHeader.frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD;
      frameHeader.codec_mode = ( vs_vocamr_codec_mode_t )codec_mode;
      rc = voice_util_get_homing_frame( gva_map_vocoder_type_gsm_to_vs ( session_obj->vocoder_type ), 
             &frameHeader, gsm_buffer->frame, gsm_buffer->size, &gsm_buffer->size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: (E)AMR: DELIVER_UL_PACKET: could not get valid homing frame rc[%d]", rc );
        break;
      }

      gsm_frame_info = ( gsm_ivocamr_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->frame_type = GSM_IVOCAMR_FRAME_TYPE_SPEECH_GOOD;
      gsm_frame_info->codec_mode = session_obj->codec_mode;
      gsm_buffer->vocoder_id = session_obj->vocoder_type;
      gsm_buffer->flags = TRUE;

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)&frameHeader, gsm_buffer );
      break;
    }

    if ( NULL != session_obj->vs_read_buf )
    {
      session_obj->enc_packet_miss_count = 0;
      vsBuffer = session_obj->vs_read_buf;
      if ( ( vsBuffer->media_id != VS_VOCAMR_MEDIA_ID ) &&
           ( vsBuffer->media_id != VS_VOCEAMR_MEDIA_ID ) )
      {
        rc = APR_EUNEXPECTED;
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: (E)AMR: DELIVER_UL_PACKET: Invalid MediaId=(0x%08x)", vsBuffer->media_id );
        break;
      }

      frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
      frameHeader = (vs_vocamr_frame_header_t*)frame;
      rc = gva_map_vocamr_frame_type_vs_to_gsm ( frameHeader->frame_type, &frame_type );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: (E)AMR: DELIVER_UL_PACKET:(): Unsupported FrameType=(%d)",
               frameHeader->frame_type );
        break;
      }

      rc = gva_map_vocamr_codec_mode_vs_to_gsm ( frameHeader->codec_mode, &codec_mode);
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: (E)AMR: DELIVER_UL_PACKET: Unsupported FrameType=(%d)",
               frameHeader->codec_mode );
        break;
      }

      /* Validate whether the frame can be fit in to client provided buffer */
      if ( vsBuffer->frame_size > gsm_buffer->size )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: (E)AMR: Invalid frame size(%d) passed by client for codec_mode(%d) expected size(%d)", 
               gsm_buffer->size, frameHeader->codec_mode, vsBuffer->frame_size );
        break;
      }

      MSG_3(MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "GVA: (E)AMR: DELIVER_UL_PACKET: FrameType=(%d), CodecMode=(%d), frameSize=(%d)",
             frameHeader->frame_type, frameHeader->codec_mode, vsBuffer->frame_size );

      gsm_frame_info = ( gsm_ivocamr_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->frame_type = frame_type;
      gsm_frame_info->codec_mode = codec_mode;
      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_AMR;
      gsm_buffer->size = vsBuffer->frame_size-1; //subtract header size 1-byte
      gsm_buffer->flags = TRUE;
      mmstd_memcpy( gsm_buffer->frame, gsm_buffer->size, (frame+1), gsm_buffer->size );

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)frameHeader, gsm_buffer );
    }

    break;
  }

  return rc;
}

static uint32_t gva_deliver_ul_packet_amrwb( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivocamrwb_frame_info_t* gsm_frame_info = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  vs_vocamrwb_frame_header_t* frameHeader = NULL;
  uint8_t* frame = NULL;
  uint32_t frame_type;
  uint32_t codec_mode = 0;

  for( ;; )
  {
    if( NULL == session_obj->vs_read_buf )
    {
      /* Send silence frame on uplink if number of encoder missed crosses a 
       * threshold. */
      session_obj->enc_packet_miss_count++;
      if( session_obj->enc_packet_miss_count <= GVA_ENC_PKT_MISS_THRESHOLD ) break;

      rc = gva_map_vocamrwb_codec_mode_gsm_to_vs ( session_obj->codec_mode, &codec_mode );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR-WB: DELIVER_UL_PACKET: Unsupported codec mode =(%d)",
               session_obj->codec_mode );
        break;
      }

      vs_vocamrwb_frame_header_t frameHeader;
      frameHeader.frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
      frameHeader.codec_mode = ( vs_vocamrwb_codec_mode_t )codec_mode;
      rc = voice_util_get_homing_frame( gva_map_vocoder_type_gsm_to_vs ( session_obj->vocoder_type ), 
             &frameHeader, gsm_buffer->frame, gsm_buffer->size, &gsm_buffer->size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR-WB: DELIVER_UL_PACKET: could not get valid homing frame rc[%d]", rc );
        break;
      }

      gsm_frame_info = ( gsm_ivocamrwb_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->frame_type = GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
      gsm_frame_info->codec_mode = session_obj->codec_mode;
      gsm_buffer->vocoder_id = session_obj->vocoder_type;
      gsm_buffer->flags = TRUE;

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)&frameHeader, gsm_buffer );
      break;
    }

    if ( NULL != session_obj->vs_read_buf )
    {
      session_obj->enc_packet_miss_count = 0;
      vsBuffer = session_obj->vs_read_buf;
      if ( vsBuffer->media_id != VS_VOCAMRWB_MEDIA_ID )
      {
        rc = APR_EUNEXPECTED;
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR-WB: DELIVER_UL_PACKET: Invalid MediaId=(0x%08x)", vsBuffer->media_id );
        break;
      }

      frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
      frameHeader = (vs_vocamrwb_frame_header_t*)frame;
      rc = gva_map_vocamrwb_frame_type_vs_to_gsm ( frameHeader->frame_type, &frame_type );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR-WB: DELIVER_UL_PACKET:(): Unsupported FrameType=(%d)",
               frameHeader->frame_type );
        break;
      }

      rc = gva_map_vocamrwb_codec_mode_vs_to_gsm ( frameHeader->codec_mode, &codec_mode);
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR-WB: DELIVER_UL_PACKET: Unsupported FrameType=(%d)",
               frameHeader->codec_mode );
        break;
      }

      /* Validate whether the frame can be fit in to client provided buffer */
      if ( vsBuffer->frame_size > gsm_buffer->size )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: AMR-WB: Invalid frame size(%d) passed by client for codec_mode(%d) expected size(%d)", 
               gsm_buffer->size, frameHeader->codec_mode, vsBuffer->frame_size );
        break;
      }

      MSG_3(MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "GVA: AMR-WB: DELIVER_UL_PACKET: FrameType=(%d), CodecMode=(%d), frameSize=(%d)",
             frameHeader->frame_type, frameHeader->codec_mode, vsBuffer->frame_size );

      gsm_frame_info = ( gsm_ivocamrwb_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->frame_type = frame_type;
      gsm_frame_info->codec_mode = codec_mode;
      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_AMRWB;
      gsm_buffer->size = vsBuffer->frame_size-1; //subtract header size 1-byte
      gsm_buffer->flags = TRUE;
      mmstd_memcpy( gsm_buffer->frame, gsm_buffer->size, (frame+1), gsm_buffer->size );

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)frameHeader, gsm_buffer );
    }

    break;
  }

  return rc;
}

static uint32_t gva_deliver_ul_packet_efr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivocefr_frame_info_t* gsm_frame_info = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  vs_vocefr_frame_header_t* frameHeader = NULL;
  uint8_t* frame = NULL;

  for( ;; )
  {
    if( NULL == session_obj->vs_read_buf )
    {
      /* Send silence frame on uplink if number of encoder missed crosses a 
       * threshold. */
      session_obj->enc_packet_miss_count++;
      if( session_obj->enc_packet_miss_count <= GVA_ENC_PKT_MISS_THRESHOLD ) break;

      gsm_frame_info = ( gsm_ivocefr_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->bfi = 0;
      gsm_frame_info->sid = 0;
      gsm_frame_info->taf = 0;
      rc = voice_util_get_homing_frame( gva_map_vocoder_type_gsm_to_vs ( session_obj->vocoder_type ), 
             gsm_frame_info, gsm_buffer->frame, gsm_buffer->size, &gsm_buffer->size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: EFR: DELIVER_UL_PACKET: could not get valid homing frame. rc[%d]", rc );
        break;
      }

      /* Update the vocoder id to UL GSM buffer. */
      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_EFR;
      gsm_buffer->flags = TRUE;

      /* Log the silence frame. */
      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)&frameHeader, gsm_buffer );
      break;
    }

    if ( NULL != session_obj->vs_read_buf )
    {
      session_obj->enc_packet_miss_count = 0;
      vsBuffer = session_obj->vs_read_buf;
      if ( vsBuffer->media_id != VS_VOCEFR_MEDIA_ID )
      {
        rc = APR_EUNEXPECTED;
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: EFR: DELIVER_UL_PACKET: Invalid MediaId[0x%08x]",
               vsBuffer->media_id );
        break;
      }

      /* Validate whether the frame can be fit in to client provided buffer */
      if ( vsBuffer->frame_size > gsm_buffer->size )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: EFR: Invalid frame size(%d) passed by client. Expected size(%d)", 
               gsm_buffer->size, vsBuffer->frame_size );
        break;
      }

      frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
      frameHeader = (vs_vocefr_frame_header_t*)frame;
      gsm_frame_info = ( gsm_ivocefr_frame_info_t* ) gsm_buffer->frame_info;

      /* Populate UL GSM buffer. */
      gsm_frame_info->bfi = frameHeader->bfi;
      gsm_frame_info->sid = frameHeader->sid;
      gsm_frame_info->taf = frameHeader->taf;
      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_EFR;
      gsm_buffer->flags = TRUE;
      gsm_buffer->size = vsBuffer->frame_size-1;//subtract header size 1-byte
      mmstd_memcpy( gsm_buffer->frame, gsm_buffer->size, (frame+1), gsm_buffer->size );

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)frameHeader, gsm_buffer );

    }

    break;
  }

  return rc;
}

static uint32_t gva_deliver_ul_packet_fr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  gsm_ivocfr_frame_info_t* gsm_frame_info = NULL;
  vs_vocfr_frame_header_t* frameHeader = NULL;
  uint8_t* frame = NULL;

  for( ;; )
  {
    if( NULL == session_obj->vs_read_buf )
    {
      /* Send silence frame on uplink if number of encoder missed crosses a 
       * threshold. */
      session_obj->enc_packet_miss_count++;
      if( session_obj->enc_packet_miss_count <= GVA_ENC_PKT_MISS_THRESHOLD ) break;

     gsm_frame_info = ( gsm_ivocfr_frame_info_t* ) gsm_buffer->frame_info;
     gsm_frame_info->bfi = 0;
     gsm_frame_info->sid = 0;
     gsm_frame_info->taf = 0;
     rc = voice_util_get_homing_frame( gva_map_vocoder_type_gsm_to_vs ( session_obj->vocoder_type ), 
            gsm_frame_info, gsm_buffer->frame, gsm_buffer->size, &gsm_buffer->size );
     if ( rc )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "GVA: FR: DELIVER_UL_PACKET: could not get valid homing frame. rc[%d]", rc );
       break;
     }

     /* Populate UL GSM buffer. */
     gsm_buffer->vocoder_id = GSM_IVOCODER_ID_FR;
     gsm_buffer->flags = TRUE;

     /* Log the silence frame. */
     gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                            (uint8_t*)&frameHeader, gsm_buffer );
     break;
   }

   if ( NULL != session_obj->vs_read_buf )
   {
     session_obj->enc_packet_miss_count = 0;
     vsBuffer = session_obj->vs_read_buf;
     if ( vsBuffer->media_id != VS_VOCFR_MEDIA_ID )
     {
       rc = APR_EUNEXPECTED;
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "GVA: FR: DELIVER_UL_PACKET: Invalid MediaId[0x%08x]",vsBuffer->media_id );
       break;
     }

      /* Validate whether the frame can be fit in to client provided buffer */
      if ( vsBuffer->frame_size > gsm_buffer->size )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: FR: Invalid frame size(%d) passed by client. Expected size(%d)", 
               gsm_buffer->size, vsBuffer->frame_size );
        break;
      }

      frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
      frameHeader = (vs_vocfr_frame_header_t*)frame;
      gsm_frame_info = ( gsm_ivocfr_frame_info_t* ) gsm_buffer->frame_info;

      //Populate the encoder data.
      gsm_frame_info->bfi = frameHeader->bfi;
      gsm_frame_info->sid = frameHeader->sid;
      gsm_frame_info->taf = frameHeader->taf;
      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_FR;
      gsm_buffer->flags = TRUE;
      gsm_buffer->size = vsBuffer->frame_size-1;//subtract header size 1-byte
      mmstd_memcpy( gsm_buffer->frame, gsm_buffer->size, (frame+1), gsm_buffer->size );

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)frameHeader, gsm_buffer );
    }

    break;
  }

  return rc;
}

static uint32_t gva_deliver_ul_packet_hr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivochr_frame_info_t* gsm_frame_info = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  vs_vochr_frame_header_t* frameHeader = NULL;
  uint8_t* frame = NULL;

  for( ;; )
  {
    if( NULL == session_obj->vs_read_buf )
    {
      /* Send silence frame on uplink if number of encoder missed crosses a 
       * threshold. */
      session_obj->enc_packet_miss_count++;
      if( session_obj->enc_packet_miss_count <= GVA_ENC_PKT_MISS_THRESHOLD ) break;

      gsm_frame_info = ( gsm_ivochr_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->bfi = 0;
      gsm_frame_info->sid = 0;
      gsm_frame_info->taf = 0;
      gsm_frame_info->ufi = 0;
      rc = voice_util_get_homing_frame( gva_map_vocoder_type_gsm_to_vs ( session_obj->vocoder_type ), 
             gsm_frame_info, gsm_buffer->frame, gsm_buffer->size, &gsm_buffer->size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: HR: DELIVER_UL_PACKET: could not get valid homing frame. rc[%d]", rc );
        break;
      }

      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_HR;
      gsm_buffer->flags = TRUE;

      /* Log the silence frame. */
      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)&frameHeader, gsm_buffer );
      break;
    }

    if ( NULL != session_obj->vs_read_buf )
    {
      session_obj->enc_packet_miss_count = 0;
      vsBuffer = session_obj->vs_read_buf;

      if ( vsBuffer->media_id != VS_VOCHR_MEDIA_ID )
      {
        rc = APR_EUNEXPECTED;
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: HR: DELIVER_UL_PACKET: Invalid MediaId[0x%08x]", vsBuffer->media_id );
        break;
      }

      /* Validate whether the frame can be fit in to client provided buffer */
      if ( vsBuffer->frame_size > gsm_buffer->size )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: HR: Invalid frame size(%d) passed by client. Expected size(%d)", 
               gsm_buffer->size, vsBuffer->frame_size );
        break;
      }

      frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
      frameHeader = (vs_vochr_frame_header_t*)frame;
      gsm_frame_info = ( gsm_ivochr_frame_info_t* ) gsm_buffer->frame_info;
      gsm_frame_info->bfi = frameHeader->bfi;
      gsm_frame_info->sid = frameHeader->sid;
      gsm_frame_info->taf = frameHeader->taf;
      gsm_frame_info->ufi = frameHeader->ufi;
      gsm_buffer->vocoder_id = GSM_IVOCODER_ID_HR;
      gsm_buffer->size = vsBuffer->frame_size-1;
      gsm_buffer->flags = TRUE;
      mmstd_memcpy( gsm_buffer->frame, gsm_buffer->size, (frame+1), gsm_buffer->size );

      gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                             (uint8_t*)frameHeader, gsm_buffer );
    }

    break;
  }

  return rc;
}

static uint32_t gva_get_dl_packet_amr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivocamr_frame_info_t* gsm_frame_info = NULL;
  vs_vocamr_frame_header_t* frameHeader = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame=NULL;
  uint32_t frame_type;
  uint32_t codec_mode;

  for( ;; )
  {
    if ( gsm_buffer->vocoder_id != GSM_IVOCODER_ID_AMR )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: (E)AMR: GET_DL_PACKET: Invalid MediaId=(0x%08x)",
             gsm_buffer->vocoder_id );
      break;
    }

    rc = gva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      GVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    vsBuffer = session_obj->vs_write_buf;
    gsm_frame_info = ( gsm_ivocamr_frame_info_t* ) gsm_buffer->frame_info;
    rc = gva_map_vocamr_frame_type_gsm_to_vs ( gsm_frame_info->frame_type, &frame_type );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: (E)AMR: GET_DL_PACKET: Unsupported FrameType=(%d),",
             gsm_frame_info->frame_type );
      break;
    }

    rc = gva_map_vocamr_codec_mode_gsm_to_vs ( gsm_frame_info->codec_mode, &codec_mode );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: (E)AMR: GET_DL_PACKET: Unsupported CodecMode = (%d),",
             gsm_frame_info->codec_mode );
      break;
    }

    vsBuffer->timestamp_us = 0;
    vsBuffer->status_mask = 0;
    vsBuffer->media_id = gva_map_vocoder_type_gsm_to_vs ( session_obj->vocoder_type );
    vsBuffer->frame_size = gsm_buffer->size+1;//add header size of 1-byte

    frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
    frameHeader = (vs_vocamr_frame_header_t*)frame;
    frameHeader->frame_type = ( vs_vocamr_frame_type_t ) frame_type;
    frameHeader->codec_mode = ( vs_vocamr_codec_mode_t ) codec_mode;
    mmstd_memcpy( frame+1, gsm_buffer->size, gsm_buffer->frame, gsm_buffer->size );

    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "GVA: (E)AMR: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d), frameSize=(%d)",
           frameHeader->frame_type, frameHeader->codec_mode, vsBuffer->frame_size );

    gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                           (uint8_t*)frameHeader, gsm_buffer );

    break;
  }

  return rc;
}


static uint32_t gva_get_dl_packet_amrwb( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivocamrwb_frame_info_t* gsm_frame_info = NULL;
  vs_vocamr_frame_header_t* frameHeader = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame=NULL;
  uint32_t frame_type;
  uint32_t codec_mode;


  for( ;; )
  {
    if ( gsm_buffer->vocoder_id != GSM_IVOCODER_ID_AMRWB )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: AMRWB: GET_DL_PACKET: Invalid MediaId=(0x%08x)",
             gsm_buffer->vocoder_id );
      break;
    }

    rc = gva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      GVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    vsBuffer = session_obj->vs_write_buf;
    gsm_frame_info = ( gsm_ivocamrwb_frame_info_t* ) gsm_buffer->frame_info;
    rc = gva_map_vocamrwb_frame_type_gsm_to_vs ( gsm_frame_info->frame_type, &frame_type );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: AMRWB: GET_DL_PACKET: Unsupported FrameType=(%d),",
             gsm_frame_info->frame_type );
      break;
    }

    rc = gva_map_vocamrwb_codec_mode_gsm_to_vs ( gsm_frame_info->codec_mode, &codec_mode );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: AMRWB: GET_DL_PACKET: Unsupported CodecMode=(%d),",
             gsm_frame_info->codec_mode );
      break;
    }

    /* Populate the decoder buffer. */
    vsBuffer->timestamp_us = 0;
    vsBuffer->status_mask = 0;
    vsBuffer->media_id = VS_VOCAMRWB_MEDIA_ID;
    vsBuffer->frame_size = gsm_buffer->size+1;//add header size of 1-byte

    frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
    frameHeader = (vs_vocamr_frame_header_t*)frame;
    frameHeader->frame_type = ( vs_vocamr_frame_type_t ) frame_type;
    frameHeader->codec_mode = ( vs_vocamr_codec_mode_t ) codec_mode;
    mmstd_memcpy( frame+1, gsm_buffer->size, gsm_buffer->frame, gsm_buffer->size );

    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "GVA: AMR-WB: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d), frameSize=(%d)",
           frameHeader->frame_type, frameHeader->codec_mode, vsBuffer->frame_size );

    gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                           (uint8_t*)frameHeader, gsm_buffer );

    break;
  }

  return rc;
}


static uint32_t gva_get_dl_packet_efr ( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_vocefr_frame_header_t* frameHeader = NULL;
  gsm_ivocefr_frame_info_t* frameInfo = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame=NULL;

  for( ;; )
  {
    if ( gsm_buffer->vocoder_id != GSM_IVOCODER_ID_EFR )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: EFR: GET_DL_PACKET: Invalid MediaId=(0x%08x)",
             gsm_buffer->vocoder_id );
      break;
    }

    rc = gva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      GVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    vsBuffer = session_obj->vs_write_buf;
    frameInfo = (gsm_ivocefr_frame_info_t*)gsm_buffer->frame_info;
    frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
    frameHeader = (vs_vocefr_frame_header_t*)frame;

    vsBuffer->media_id = VS_VOCEFR_MEDIA_ID;
    frameHeader->bfi = frameInfo->bfi;
    frameHeader->sid = frameInfo->sid;
    frameHeader->taf = frameInfo->taf;
    vsBuffer->frame_size = gsm_buffer->size+1;//add header size of 1-byte
    mmstd_memcpy( frame+1, gsm_buffer->size, gsm_buffer->frame, gsm_buffer->size );

    gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                           (uint8_t*)frameHeader, gsm_buffer );

    break;
  }

  return rc;
}

static uint32_t gva_get_dl_packet_fr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_vocfr_frame_header_t* frameHeader = NULL;
  gsm_ivocfr_frame_info_t* frameInfo = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame=NULL;

  for( ;; )
  {
    if ( gsm_buffer->vocoder_id != GSM_IVOCODER_ID_FR )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: FR: GET_DL_PACKET: Invalid MediaId=(0x%08x)",
             gsm_buffer->vocoder_id );
      break;
    }

    rc = gva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      GVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    vsBuffer = session_obj->vs_write_buf;
    frameInfo = (gsm_ivocfr_frame_info_t*)gsm_buffer->frame_info;
    frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
    frameHeader = (vs_vocfr_frame_header_t*)frame;

    vsBuffer->media_id = VS_VOCFR_MEDIA_ID;
    frameHeader->bfi = frameInfo->bfi;
    frameHeader->sid = frameInfo->sid;
    frameHeader->taf = frameInfo->taf;
    vsBuffer->frame_size = gsm_buffer->size+1;//add header size of 1-byte
    mmstd_memcpy( frame+1, gsm_buffer->size, gsm_buffer->frame, gsm_buffer->size );

    gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                           (uint8_t*)frameHeader, gsm_buffer );

    break;
  }

  return rc;
}

static uint32_t gva_get_dl_packet_hr( 
  gsm_ivocoder_buffer_t* gsm_buffer,
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_vochr_frame_header_t* frameHeader = NULL;
  gsm_ivochr_frame_info_t* frameInfo = NULL;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame=NULL;

  for( ;; )
  {
    if ( gsm_buffer->vocoder_id != GSM_IVOCODER_ID_HR )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: HR: GET_DL_PACKET: Invalid MediaId=(0x%08x)",
             gsm_buffer->vocoder_id );
      break;
    }

    rc = gva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      GVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    vsBuffer = session_obj->vs_write_buf;
    frameInfo = (gsm_ivochr_frame_info_t*)gsm_buffer->frame_info;
    frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );
    frameHeader = (vs_vochr_frame_header_t*)frame;

    vsBuffer->media_id = VS_VOCHR_MEDIA_ID;
    frameHeader->bfi = frameInfo->bfi;
    frameHeader->sid = frameInfo->sid;
    frameHeader->taf = frameInfo->taf;
    frameHeader->ufi = frameInfo->ufi;
    vsBuffer->frame_size = gsm_buffer->size+1;//add header size of 1-byte
    mmstd_memcpy( frame+1, gsm_buffer->size, gsm_buffer->frame, gsm_buffer->size );

    gva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                           (uint8_t*)frameHeader, gsm_buffer );

    break;
  }

  return rc;
}

/****************************************************************************
 * GVA GSM SESSION ROUTINES                                                 *
 ****************************************************************************/

static uint32_t gva_gsm_event_cb(
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
)
{
  uint32_t rc = APR_EOK;

  switch ( event_id )
  {
   case GSM_IVOICE_EVENT_START_REQUEST:
   case GSM_IVOICE_EVENT_STOP_REQUEST:
   case GSM_IVOICE_EVENT_SELECT_OWNER:
   case GSM_IVOICE_EVENT_REQUEST_CODEC_MODE:
   case GSM_IVOICE_EVENT_REQUEST_DTX_MODE:
    (void) gva_log_event_info( session_context, event_id );
     rc = gva_prepare_and_dispatch_cmd_packet( 
            session_context, event_id, params, size );
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "GVA: Unsupported EventId=(%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static void gva_gsm_vocoder_ul_cb(
 void* session_context,
 gsm_ivocoder_buffer_t* gsm_buffer
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;

  session_obj = ( gva_session_object_t* )session_context;
  if( session_obj == NULL ) return;

  gsm_buffer->flags = FALSE;

  GVA_ACQUIRE_LOCK( session_obj->data_lock );

  if(TRUE == session_obj->is_vs_ready)
  {
    rc = gva_vs_get_encoder_buffer ( session_obj );
  }
  
  if ( NULL != session_obj->vs_read_buf )
  {
    ( void ) gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RUNNING );
  }

  switch ( session_obj->vocoder_type )
  {
    case GSM_IVOCODER_ID_AMR:
      rc = gva_deliver_ul_packet_amr( gsm_buffer, session_obj );
      break;

    case GSM_IVOCODER_ID_AMRWB:
      rc = gva_deliver_ul_packet_amrwb( gsm_buffer, session_obj );
      break;

    case GSM_IVOCODER_ID_EFR:
      rc = gva_deliver_ul_packet_efr( gsm_buffer, session_obj );
      break;

    case GSM_IVOCODER_ID_FR:
      rc = gva_deliver_ul_packet_fr( gsm_buffer, session_obj );
      break;

    case GSM_IVOCODER_ID_HR:
      rc = gva_deliver_ul_packet_hr( gsm_buffer, session_obj );
      break;

    default:
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "GVA: Unsupported VocoderID=(0x%08X)", session_obj->vocoder_type );
      break;
  }

  if ( FALSE == gsm_buffer->flags ) 
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "GVA: UL vocoder Packet not transfered to GSM, is_vs_ready=(%d), "
         "vs_read_buf=(0x%08x)", session_obj->is_vs_ready, session_obj->vs_read_buf );
  }

  ( void ) gva_vs_return_encoder_buffer( session_obj );

  GVA_RELEASE_LOCK( session_obj->data_lock );

  return;
}

static void gva_gsm_vocoder_dl_cb (
  void* session_context,
  gsm_ivocoder_buffer_t* gsm_buffer
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  session_obj = ( gva_session_object_t* )session_context;
  if( session_obj == NULL ) return;

  GVA_ACQUIRE_LOCK( session_obj->data_lock );

  if ( ( TRUE == session_obj->is_vs_ready ) && ( TRUE == gsm_buffer->flags ) )
  {
    /* Pass the vocoder packet content avaialble form GSM protocol to VS. */
    switch ( session_obj->vocoder_type )
    {
      case GSM_IVOCODER_ID_AMR:
        rc = gva_get_dl_packet_amr( gsm_buffer, session_obj );
        break;

      case GSM_IVOCODER_ID_AMRWB:
        rc = gva_get_dl_packet_amrwb( gsm_buffer, session_obj );
        break;

      case GSM_IVOCODER_ID_EFR:
        rc = gva_get_dl_packet_efr( gsm_buffer, session_obj );
        break;

      case GSM_IVOCODER_ID_FR:
        rc = gva_get_dl_packet_fr( gsm_buffer, session_obj );
        break;

      case GSM_IVOCODER_ID_HR:
        rc = gva_get_dl_packet_hr( gsm_buffer, session_obj );
        break;

      default:
        rc = APR_EUNEXPECTED;
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "GVA: Unsupported VocoderID=(0x%08X)", session_obj->vocoder_type );
        break;
    }

    /* Pass write buffer to VS, if dl packet processed successfully. */
    if ( rc == APR_EOK )
    {
      // Flush stale downlink data
      flushCmd.handle = session_obj->vs_handle;
      flushCmd.enc_flush = FALSE; flushCmd.dec_flush = TRUE;
      rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
      GVA_REPORT_FATAL_ON_ERROR(rc);

      ( void ) gva_vs_put_decoder_buffer( session_obj );
    }
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "GVA: DL vocoder packet not tranfered to VS, is_vs_ready = (%d), "
           "gsm_buffer->flags=(0X%08X)", session_obj->is_vs_ready, gsm_buffer->flags );
  }

  GVA_RELEASE_LOCK( session_obj->data_lock );

  return;
}

static uint32_t gva_gsm_open_session (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivoice_cmd_open_t open_cmd;

  open_cmd.ret_handle = &session_obj->gsm_handle;
  open_cmd.asid = session_obj->asid;
  open_cmd.event_cb = gva_gsm_event_cb;
  open_cmd.ul_cb = gva_gsm_vocoder_ul_cb;
  open_cmd.dl_cb = gva_gsm_vocoder_dl_cb;  
  open_cmd.session_data = ( void* ) session_obj;
  rc = gsm_call ( GSM_IVOICE_CMD_OPEN, &open_cmd, sizeof( open_cmd ) );
  if ( ( APR_EOK != rc ) && ( APR_EUNSUPPORTED != rc ) ) 
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: Failed to open GSM session for asid[%d]) with rc[0x%08x]",
           open_cmd.asid, rc );
  }
  else
  {
    rc = VS_EOK;
  }

  return rc;
}

static uint32_t gva_gsm_close_session (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivoice_cmd_close_t close_cmd;

  close_cmd.handle = session_obj->gsm_handle;
  rc = gsm_call( GSM_IVOICE_CMD_CLOSE, &close_cmd, sizeof( close_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: failed to close GSM session for asid[%d] with rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

static uint32_t gva_gsm_start_session (
 gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivoice_cmd_start_t start_cmd;

  start_cmd.handle = session_obj->gsm_handle;
  rc = gsm_call ( GSM_IVOICE_CMD_START, &start_cmd, sizeof( start_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: failed to start GSM session for asid[%d] with rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

static uint32_t gva_gsm_stop_session (
  gva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  gsm_ivoice_cmd_stop_t stop_cmd;

  stop_cmd.handle = session_obj->gsm_handle;
  rc = gsm_call ( GSM_IVOICE_CMD_STOP, &stop_cmd, sizeof( stop_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "GVA: failed to stop GSM session for asid[%d] with rc[0x%08x]",
           session_obj->asid, rc );
  }

  return rc;
}

/****************************************************************************
 * GVA CMDs/EVENTs HANDLING ROUTINES                                        *
 ****************************************************************************/

static uint32_t gva_process_vs_cmd_response_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  gva_simple_job_object_t* simple_obj = NULL;
  vs_common_event_cmd_response_t* evt_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_common_event_cmd_response_t, packet );
    if( evt_params == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "GVA: cmd response recieved for cmd_id[0x%08x], token[0x%08x], status[0x%08X]",
           evt_params->cmd_id, evt_params->token, evt_params->status );

    simple_obj = ( gva_simple_job_object_t* ) evt_params->token;
    if ( simple_obj == NULL ) break;

    simple_obj->is_completed = TRUE;
    simple_obj->status = evt_params->status;

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t gva_process_vs_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = TRUE;
    gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_READY );

    GVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t gva_process_vs_not_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    if( APR_NULL_V != session_obj->vs_handle )
    {
      flushCmd.handle = session_obj->vs_handle;
      flushCmd.enc_flush = TRUE;
      flushCmd.dec_flush = TRUE;
      rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
      GVA_REPORT_FATAL_ON_ERROR(rc);
    }

    session_obj->is_vs_ready = FALSE;
    gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_NOT_READY );

    GVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t gva_process_vs_eamr_rate_change_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  vs_voc_event_operating_mode_update_t* evt_params = NULL;
  gsm_ivoice_cmd_send_sample_rate_t sample_rate_cmd;

  for( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
 
    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
    
    evt_params = APRV2_PKT_GET_PAYLOAD( vs_voc_event_operating_mode_update_t, packet );
    if( evt_params == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
    
    if( session_obj->vocoder_type != GSM_IVOCODER_ID_AMR )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "gva_process_vs_eamr_rate_change_event(): Not Applicable for "
             "vocoder_type=0x%08x", session_obj->vocoder_type );
      break;
    }

    switch ( evt_params->rx_mode )
    {
      case VS_VOC_BANDWIDTH_NB:
        sample_rate_cmd.sample_rate = 8000;
        break;

      case VS_VOC_BANDWIDTH_WB:
        sample_rate_cmd.sample_rate = 16000;
        break;

      default:
        sample_rate_cmd.sample_rate = 0;
        break;
    }

    if( ( evt_params->rx_mode == VS_VOC_BANDWIDTH_NB ) ||
        ( evt_params->rx_mode == VS_VOC_BANDWIDTH_WB ) )
    {
      sample_rate_cmd.handle = session_obj->gsm_handle;
      sample_rate_cmd.vocoder_id = session_obj->vocoder_type;
      ( void ) gsm_call( GSM_IVOICE_CMD_SEND_SAMPLE_RATE, &sample_rate_cmd, sizeof( sample_rate_cmd ) );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "GVA: sample rate change notification for asid[%d] rx_mode[%d] sample_rate[%d]",
             session_obj->asid, evt_params->rx_mode, sample_rate_cmd.sample_rate );
    }

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t gva_process_gsm_dtx_mode_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  gsm_ivoice_event_request_dtx_mode_t* evt_params = NULL;

  for( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( gsm_ivoice_event_request_dtx_mode_t, packet );
    if( evt_params == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->dtx_mode = evt_params->enable_flag;
    gva_vs_set_voc_dtx_mode ( session_obj );

    GVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t gva_process_gsm_select_owner_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    /* FOR IRAT-HO Scenrios,request voice agent for resource grant. */
    if ( session_obj->is_resource_granted == FALSE ) 
    {
      requestResourceCmd.asid = session_obj->asid;
      requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_GSM;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                        sizeof(requestResourceCmd) );
      GVA_REPORT_FATAL_ON_ERROR(rc);

      gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
    }

    GVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t gva_process_gsm_codec_mode_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  gva_session_object_t* session_obj = NULL;
  gsm_ivoice_event_request_codec_mode_t* evt_params = NULL;

  for( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( gsm_ivoice_event_request_codec_mode_t, packet );
    if( evt_params == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->codec_mode = evt_params->codec_mode;
    gva_vs_set_voc_codec_mode ( session_obj );

    GVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( gva_apr_handle, packet );
  GVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t gva_process_gsm_vocoder_start_event( 
 gva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  gva_session_object_t* session_obj = NULL;
  gsm_ivoice_event_start_request_t* evt_params = NULL;
  gva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      rc = APR_EUNEXPECTED;
      GVA_REPORT_FATAL_ON_ERROR ( rc );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( gsm_ivoice_event_start_request_t, packet );
    if( evt_params == NULL )
    {
      rc = APR_EUNEXPECTED;
      GVA_REPORT_FATAL_ON_ERROR ( rc );
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == GVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "gva_process_gsm_vocoder_start_event() - GSM configured with "
             "vocoder_id = (0x%08x), new vocoder id = (0x%08x)",
             session_obj->vocoder_type, evt_params->vocoder_id );

      if ( session_obj->is_gsm_ready == TRUE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "gva_process_gsm_vocoder_start_event() - GSM already sent IVOICE_EVENT_START_REQUEST" );
        break;
      }

      if( gva_validate_gsm_vocoder_id( evt_params->vocoder_id ) == FALSE )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "gva_process_gsm_vocoder_start_event() - Invalid vocoder Id",
               evt_params->vocoder_id );
        break;
      }

      /* Update the GVA call state. */
      session_obj->is_gsm_ready = TRUE;
      session_obj->vocoder_type = evt_params->vocoder_id;
       /* Initialzing the miss count with the threshold so that GVA can push the homing sequence
         straight away during call start and handover scenarios. Miss count will be reset to zero when the
         first encoder packet arrives, so the threshold will come into play only when there are
         packet misses during steady state.
       */
      session_obj->enc_packet_miss_count = GVA_ENC_PKT_MISS_THRESHOLD;

      if ( session_obj->is_resource_granted == FALSE )
      {
        /* Request for voice resource. */
        requestResourceCmd.asid = session_obj->asid;
        requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_GSM;
        rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                          sizeof(requestResourceCmd) );
        GVA_REPORT_FATAL_ON_ERROR(rc);

        gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
        break;
      }

      /* Create the Simple job object to track VS_ENABLE. */
      rc = gva_create_simple_job_object( session_obj->header.handle,
             ( gva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      /* Set cached vocoder properties. */
      rc = gva_vs_open_vocoder_session( session_obj );
      GVA_PANIC_ON_ERROR(rc);

      gva_vs_set_voc_media_type( session_obj );
      gva_vs_set_voc_dtx_mode( session_obj );
      gva_vs_set_voc_codec_mode( session_obj );
      gva_vs_set_voc_timing_offsets( session_obj );

      rc = gva_vs_enable_vocoder( session_obj, simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
        break;
      }

      /* Send a START CMD notification to GSM. */
      ( void ) gva_gsm_start_session ( session_obj );
    }

    if ( VS_EOK == rc ) 
    {
      /* Free CMD/EVT packet memory. */
      ( void ) gva_mem_free_object ( ( gva_object_t*) simple_obj );
    }

    break;
  }

  if ( session_obj != NULL )
  {
    GVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t gva_process_gsm_vocoder_stop_event( 
 gva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  gva_session_object_t* session_obj = NULL;
  gva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc  = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( gva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( rc );
      rc  = APR_EUNEXPECTED;
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == GVA_GATING_CMD_STATE_EXECUTE )
    {
      if ( FALSE == session_obj->is_gsm_ready )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "gsm_vocoder_stop_event(PROCESS) - vocoder not started yet." );
        break;
      }

      /* Update the GVA call state. */
      session_obj->is_gsm_ready = FALSE;

      /* Clear vocoder state history. */
      session_obj->vocoder_type = GVA_VOCODER_ID_UNDEFINED_V;

      if ( TRUE == session_obj->is_resource_granted )
      {
        /* Create the Simple job object to track CVD setup. */
        rc = gva_create_simple_job_object( session_obj->header.handle,
               ( gva_simple_job_object_t** ) &ctrl->rootjob_obj );
        simple_obj = &ctrl->rootjob_obj->simple_job;

        rc = gva_vs_close_vocoder_session( session_obj, simple_obj );
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

    if ( APR_EOK == rc )
    {
      /* Send voice resource released event to voice agent.
       * "REQUEST_STOP sent by GERAN-L1 without sending REQUEST_START" 
       * This can happen if GERAN-L1 called SELECT_OWNER during interRAT 
       * handover start. However the handover failed afterwards. So, GERAN-L1 
       * did not call REQUEST_START, instead directly called REQUEST_STOP 
       * to indicate GVA that it no longer required vocoder. */
      session_obj->is_resource_granted = FALSE;
      releaseResourceCmd.asid = session_obj->asid;
      releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_GSM;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                        sizeof(releaseResourceCmd) );
      GVA_REPORT_FATAL_ON_ERROR(rc);

      gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );

      /* Send stop notification to GSM. */ 
      ( void ) gva_gsm_stop_session ( session_obj );

      ( void ) gva_mem_free_object ( ( gva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    GVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}

static uint32_t gva_va_event_cb(
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
      rc = gva_prepare_and_dispatch_cmd_packet( NULL, event_id, params, size );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "GVA: Unsupported EventId=(%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t gva_process_resource_grant_event ( 
 gva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  gva_session_object_t* session_obj = NULL;
  vagent_ivoice_event_resource_grant_t* event_params = NULL;
  gva_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_grant_t, packet );
    if( event_params == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = gva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == GVA_GATING_CMD_STATE_EXECUTE )
    {
      session_obj->is_resource_granted = TRUE;
      session_obj->vsid = event_params->vsid;
      gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_GRANTED );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "GVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_GRANT recieved",
             session_obj->asid, session_obj->vsid );

      if ( session_obj->is_gsm_ready == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "gva_process_resource_grant_event() - IVOICE_EVENT_START_REQUEST not "
             "received from GSM" );
        break;
      }

      /* Create the Simple job object to track VS_ENABLE. */
      rc = gva_create_simple_job_object( session_obj->header.handle,
             ( gva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      /* Set cached vocoder properties. */
      rc = gva_vs_open_vocoder_session( session_obj );
      GVA_PANIC_ON_ERROR(rc);
      gva_vs_set_voc_media_type( session_obj );
      gva_vs_set_voc_dtx_mode( session_obj );
      gva_vs_set_voc_codec_mode( session_obj );
      gva_vs_set_voc_timing_offsets( session_obj );

      rc = gva_vs_enable_vocoder( session_obj, simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
        break;
      }

      /* Send a START CMD notification to GSM. */
      ( void ) gva_gsm_start_session ( session_obj );
    }

    if ( rc == APR_EOK )
    {
      ( void ) gva_mem_free_object ( ( gva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    GVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t gva_process_resource_revoke_event ( 
 gva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  gva_session_object_t* session_obj = NULL;
  vagent_ivoice_event_resource_revoke_t* event_params = NULL;
  gva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if( packet == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_revoke_t, packet );
    if( event_params == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = gva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      GVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    GVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == GVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "GVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_REVOKE recieved",
             session_obj->asid, session_obj->vsid );

      /* Create a simple job object to track VS_DISABLE. */
      rc = gva_create_simple_job_object( session_obj->header.handle,
             ( gva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj= &ctrl->rootjob_obj->simple_job;

      rc = gva_vs_close_vocoder_session( session_obj, simple_obj );
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
      releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_GSM;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                        sizeof(releaseResourceCmd) );
      GVA_REPORT_FATAL_ON_ERROR(rc);

      gva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );

      ( void ) gva_mem_free_object ( ( gva_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    GVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}

/****************************************************************************
 * NONGATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/

static void gva_task_process_nongating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  gva_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;

  while( apr_list_remove_head( &gva_nongating_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &gva_free_work_pkt_q, &work_item->link );

    switch( packet->opcode )
    {
     /**
      * Handling routine for nongating work-items should take of release the
      * memory allocated for the CMD/EVENT packets.
      */
     case GSM_IVOICE_EVENT_REQUEST_CODEC_MODE:
       rc = gva_process_gsm_codec_mode_event( packet );
       break;

     case GSM_IVOICE_EVENT_REQUEST_DTX_MODE:
       rc = gva_process_gsm_dtx_mode_event( packet );
       break;

     case GSM_IVOICE_EVENT_SELECT_OWNER:
       rc = gva_process_gsm_select_owner_event( packet );
       break;

     case VS_COMMON_EVENT_CMD_RESPONSE:
       rc = gva_process_vs_cmd_response_event( packet );
       break;

     case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
       rc = gva_process_vs_eamr_rate_change_event( packet );
       break;

     case VS_VOC_EVENT_READY:
       rc = gva_process_vs_ready_event( packet );
       break;

     case VS_VOC_EVENT_NOT_READY:
       rc = gva_process_vs_not_ready_event( packet );
       break;

     default:
       /* Add remaining work items to the gating work queue. */
       rc = gva_queue_apr_packet ( packet, &gva_gating_work_pkt_q.cmd_q );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "GVA: NON-GATING: cmd pkt queuing failed rc[0x%08x]", rc );
         rc = __aprv2_cmd_free( gva_apr_handle, packet );
         GVA_REPORT_FATAL_ON_ERROR(rc);
       }

       break;
    }
  }

  return;
}

/****************************************************************************
 * GATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/
 
static void gva_task_process_gating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  gva_work_item_t* work_item;
  gva_gating_control_t* ctrl = &gva_gating_work_pkt_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
      case GVA_GATING_CMD_STATE_FETCH:
      {
        /* Fetch the next gating command to execute. */
        rc = apr_list_remove_head( &ctrl->cmd_q,
                                   ( ( apr_list_node_t** ) &work_item ) );
        if ( rc ) return;

        if( work_item->packet == NULL )
        {
          GVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
        }

        ctrl->packet = work_item->packet;
        ctrl->state = GVA_GATING_CMD_STATE_EXECUTE;

        /* Add the vs_work_item_t back to vs_free_work_pkt_q. */
        work_item->packet = NULL;
        ( void ) apr_list_add_tail( &gva_free_work_pkt_q, &work_item->link );
      }
      break;

      case GVA_GATING_CMD_STATE_EXECUTE:
      case GVA_GATING_CMD_STATE_CONTINUE:
      {
        if ( GVA_GATING_CMD_STATE_EXECUTE == ctrl->state )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "GVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) STARTED",
                 ctrl->packet->opcode);
        }

        /* For Supported request_id, handler should take care of releasing 
         * memory allocated for packets. */
        switch ( ctrl->packet->opcode )
        {
          case GSM_IVOICE_EVENT_START_REQUEST:
            rc = gva_process_gsm_vocoder_start_event( ctrl );
            break;

          case GSM_IVOICE_EVENT_STOP_REQUEST:
            rc = gva_process_gsm_vocoder_stop_event( ctrl );
            break;

          case VAGENT_IVOICE_EVENT_RESOURCE_GRANT:
            rc = gva_process_resource_grant_event( ctrl );
            break;

          case VAGENT_IVOICE_EVENT_RESOURCE_REVOKE:
            rc = gva_process_resource_revoke_event( ctrl );
            break;

          default:
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "GVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) UNSUPPORTED",
                   ctrl->packet->opcode );
            rc = APR_EOK;
            break;
        }

        /* Evaluate the pending command completion status. */
       switch ( rc )
       {
         case APR_EOK:
         {
           MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                  "GVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) ENDED",
                  ctrl->packet->opcode );
           rc = __aprv2_cmd_free( gva_apr_handle, ctrl->packet );
           GVA_REPORT_FATAL_ON_ERROR(rc)

           ctrl->packet = NULL;
           /* The current command is finished so fetch the next command. */
           ctrl->state = GVA_GATING_CMD_STATE_FETCH;
         }
         break;

         case APR_EPENDING:
           /**
            * Assuming the current pending command control routine returns
            * APR_EPENDING the overall progress stalls until one or more
            * external events or responses are received.
            */
           ctrl->state = GVA_GATING_CMD_STATE_CONTINUE;
           /**
            * Return from here so as to avoid unecessary looping till reponse
            * is recived.
            */
           return;

         default:
           GVA_PANIC_ON_ERROR( APR_EUNEXPECTED );
           break;
       }

     }
     break;

     default:
       GVA_PANIC_ON_ERROR( rc );
       break;
    }/* switch case ends. */
  }/* for loop ends. */

  return;
}

/****************************************************************************
 * GVA TASK ROUTINES                                                        *
 ****************************************************************************/

static int32_t gva_run ( void )
{
  gva_task_process_nongating_work_items( );
  gva_task_process_gating_work_items( );

  return APR_EOK;
}

static int32_t gva_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &gva_work_event );
  apr_event_signal( gva_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( gva_work_event );
    if ( rc ) break;

    gva_run( );
  }

  apr_event_destroy( gva_work_event );
  apr_event_signal( gva_control_event );

  return APR_EOK;
}


/****************************************************************************
 * GVA BOOT-UP and POWER-DOWN ROUTINES                                      *
 ****************************************************************************/

static uint32_t gva_gating_control_init (
  gva_gating_control_t* p_ctrl
)
{
  uint32_t rc = APR_EOK;

  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q,
                         gva_thread_lock_fn, gva_thread_unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  p_ctrl->state = GVA_GATING_CMD_STATE_FETCH;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return APR_EOK;
}  /* end of gva_gating_control_init () */

static uint32_t gva_gating_control_destroy (
  gva_gating_control_t* p_ctrl
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

static int32_t gva_init ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index;
  RCINIT_INFO info_handle = NULL;
  RCINIT_PRIO priority = 0;
  unsigned long stack_size = 0;
  voicecfg_cmd_get_bool_item_t item;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== gva_init(): ======" );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &gva_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &gva_thread_lock );
    apr_event_create( &gva_control_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &gva_heapmgr, ( ( void* ) &gva_heap_pool ),
                          sizeof( gva_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = gva_object_table;
    params.total_bits = GVA_HANDLE_TOTAL_BITS_V;
    params.index_bits = GVA_HANDLE_INDEX_BITS_V;
    params.lock_fn = gva_int_lock_fn;
    params.unlock_fn = gva_int_unlock_fn;
    rc = apr_objmgr_construct( &gva_objmgr, &params );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &gva_free_work_pkt_q, 
                           gva_int_lock_fn, gva_int_unlock_fn );
    for ( index = 0; index < GVA_NUM_WORK_PKTS_V; ++index )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &gva_work_pkts[index] );
      gva_work_pkts[index].packet = NULL;
      rc = apr_list_add_tail( &gva_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &gva_work_pkts[index] ) );
    }
    rc = apr_list_init_v2( &gva_nongating_work_pkt_q,
                           gva_int_lock_fn, gva_int_unlock_fn );
  }

  { /* Initialize gating work pkt queue. */
    rc = gva_gating_control_init( &gva_gating_work_pkt_q );
  }

  { /* Initialize the global session lock. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &gva_global_lock );
    GVA_PANIC_ON_ERROR ( rc );
  }

  { /* Create the GVA task worker thread. */
#if ( !defined AVS_MPSS_TEST )
    info_handle = rcinit_lookup( GVA_TASK_NAME );
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
           "gva_init(): GVA task not registered with RCINIT" );
      priority = GVA_TASK_PRIORITY;
      stack_size = GVA_TASK_STACK_SIZE;
    }

    if ( ( priority > 255 ) || ( stack_size == 0 ) ) 
    {
      ERR_FATAL( "gva_init(): Invalid priority: %d or stack size: %d",
                 priority, stack_size, 0 );
    }

    rc = apr_thread_create( &gva_thread, GVA_TASK_NAME, TASK_PRIORITY(priority),
                            gva_task_stack, stack_size, 
                            gva_worker_thread_fn , NULL );
    GVA_PANIC_ON_ERROR( rc );

    apr_event_wait( gva_control_event );
  }

  gva_is_initialized = TRUE;

  item.id = VOICECFG_CFG_IS_2G_EAMR_ENABLED;
  item.ret_value = FALSE;
  rc =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );
  if ( ! rc  && item.ret_value == TRUE )
  {
    gva_is_eamr_enabled = TRUE;
  }

  return rc;
}


static int32_t gva_postinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;
  vagent_ivoice_cmd_register_t registerCmd;

  rc = __aprv2_cmd_register2(
         &gva_apr_handle, gva_my_dns, sizeof( gva_my_dns ), 0,
         gva_isr_dispatch_fn, NULL, &gva_my_addr );
  GVA_PANIC_ON_ERROR(rc);

  /* Initialize the mapping info and open onex voice instance. */
  for( index = 0; index < GVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* Create and initialize GVA session object. */
    rc =  gva_create_session_object ( &gva_session_obj_list[index] );
    GVA_PANIC_ON_ERROR( rc );

    /* Open CDMA voice session instance. */
    gva_session_obj_list[index]->asid = ( sys_modem_as_id_e_type ) index;
    rc = gva_gsm_open_session( gva_session_obj_list[index] );
    GVA_PANIC_ON_ERROR (rc);
  }

  /* Registring with voice agent for resource. */
  registerCmd.client_id = VAGENT_VOICE_CLIENT_GSM;
  registerCmd.client_cb = gva_va_event_cb;
  registerCmd.client_data = NULL;
  rc = vagent_call( VAGENT_IVOICE_CMD_REGISTER, (void*)&registerCmd, sizeof(registerCmd) );
  GVA_PANIC_ON_ERROR (rc);

  return rc;
}


static int32_t gva_predeinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;

  /* Close gsm Session instance. */
  for( index = 0; index < GVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* GVA CLOSE's CDMA session instance for all available ASID. */
    ( void ) gva_gsm_close_session( gva_session_obj_list[index] );

    /* Free GVA session object for all VSID. */
    ( void ) apr_lock_destroy( gva_session_obj_list[index]->data_lock );
    ( void ) gva_mem_free_object( (gva_object_t*)gva_session_obj_list[ index ] );
  }

  return rc;
}

static int32_t gva_deinit ( void )
{
  uint32_t rc = APR_EOK;

  gva_is_initialized = FALSE;

  apr_event_signal_abortall( gva_work_event );
  apr_event_wait( gva_control_event );

  /* Release gating control structures */
  ( void ) gva_gating_control_destroy( &gva_gating_work_pkt_q );

  /* Release work queue */
  ( void ) apr_list_destroy( &gva_free_work_pkt_q );
  ( void ) apr_list_destroy( &gva_nongating_work_pkt_q );

  /* Deinitialize the object handle table. */
  apr_objmgr_destruct( &gva_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  ( void ) apr_event_destroy( gva_control_event );
  ( void ) apr_lock_destroy( gva_int_lock );
  ( void ) apr_lock_destroy( gva_thread_lock );
  ( void ) apr_lock_destroy( gva_global_lock );

  return rc;
}


GVA_EXTERNAL uint32_t gva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  if ( ( FALSE == gva_is_initialized ) && ( DRV_CMDID_INIT != cmd_id ) )
  {
    GVA_REPORT_FATAL_ON_ERROR( APR_ENOSERVICE );
    return APR_ENOSERVICE;
  }

  switch ( cmd_id )
  {
  case DRV_CMDID_INIT:
    rc = gva_init( );
    break;

  case DRV_CMDID_POSTINIT:
    rc = gva_postinit( );
    break;

  case DRV_CMDID_PREDEINIT:
    rc = gva_predeinit( );
    break;

  case DRV_CMDID_DEINIT:
    rc = gva_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "gva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

