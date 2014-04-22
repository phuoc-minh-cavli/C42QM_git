/*
   Copyright (C) 2015-2019 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/dev/avs.mpss/8.0/sduddell.avs.mpss.8.0.CT_mapping/vsd/vagent/src/CT/vagent.c#1
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
#include "err.h"
#include "rcinit.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_memmgr.h"
#include "apr_event.h"
#include "apr_thread.h"

/* CM APIs. */
#include "cm.h"
#include "cm_dualsim.h"
#include "sys.h"

/* VSD APIs. */
#include "drv_api.h"
#include "vs_task.h"
#include "voicelog_utils.h"

/* VADAPTER APIs. */
#include "gva_if.h"
#include "gva_iresource_if.h"

/* SELF APIs. */
#include "vagent_if.h"
#include "../../inc/CT/private/vagent_i.h"


/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Currently voice agent supports three subscriptions.
 * One VSID per subscription
 */
#define VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V ( 3 )

/*
 * Represent that number of voice sessions(CVD) that can exist concurrently 
 * at a time in the system.
 */
#define VAGENT_MAX_NUM_OF_RESOURCES_V ( 1 )

/*
 * Represents ID's associated to modules among which voice agent sequences
 * the voice resource usage.
 */
#define VAGENT_MODULE_ID_CVA   ( 1 )
#define VAGENT_MODULE_ID_GVA   ( 2 )
#define VAGENT_MODULE_ID_WVA   ( 3 )
#define VAGENT_MODULE_ID_TVA   ( 4 )
#define VAGENT_MODULE_ID_IVA   ( 5 )
#define VAGENT_MODULE_ID_ECALL ( 6 )

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

#define VAGENT_EVENTI_CM_CALL_EVENT ( 0x000131F9 )

typedef struct vagent_eventi_cm_call_event_t vagent_eventi_cm_call_event_t;

struct vagent_eventi_cm_call_event_t
{
  sys_modem_as_id_e_type asid;
    /**< modem active subcription ID. */

  cm_call_event_e_type event_id;
    /**< call events as reported by CM. */

  sys_voice_handover_e_type ho_type;
    /**< Reported handover type during HO events.. */
};

/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

static apr_lock_t vagent_int_lock;
static apr_event_t vagent_thread_create_event;

static apr_memmgr_type vagent_heapmgr;
static uint8_t vagent_heap_pool[ VAGENT_HEAP_SIZE_V ];

static apr_objmgr_t vagent_objmgr;
static apr_objmgr_object_t vagent_object_table[ VAGENT_MAX_OBJECTS_V ];

static apr_list_t vagent_work_pkt_q;
static apr_list_t vagent_free_work_pkt_q;
static vagent_work_item_t vagent_work_pkts[ VAGENT_NUM_WORK_PKTS_V ];

static apr_event_t vagent_work_event;
static apr_thread_t vagent_thread;
static uint8_t vagent_task_stack[ VAGENT_TASK_STACK_SIZE ];

static vagent_modem_subs_object_t* vagent_modem_subs_obj_list[VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V];
static vagent_session_object_t* vagent_session_obj_list[VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V];

static bool_t vagent_is_initialized = FALSE;

static uint32_t vagent_num_resources = VAGENT_MAX_NUM_OF_RESOURCES_V;
  /**< Indicates total number of voice resource available for grant. */


/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

VAGENT_INTERNAL void vagent_int_lock_fn ( 
  void
)
{

  apr_lock_enter( vagent_int_lock );

}

VAGENT_INTERNAL void vagent_int_unlock_fn (
  void
)
{

  apr_lock_leave( vagent_int_lock );

}


/****************************************************************************
 * CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                      *
*****************************************************************************/

VAGENT_INTERNAL uint32_t vagent_free_event_packet (
  vagent_event_packet_t* packet
)
{

  uint32_t rc = APR_EOK;

  if ( packet != NULL )
  {
    if( packet->params != NULL )
    {
      /* Free the memory - p_cmd_packet->params. */
      apr_memmgr_free( &vagent_heapmgr, packet->params );
      packet->params = NULL;
    }
 
    /* Free the memeory - p_cmd_packet. */
    apr_memmgr_free( &vagent_heapmgr, packet );
    packet= NULL;
  }
  return rc;

}


/**
 * This is a common routine that prepares the incoming events and dispatches 
 * (signals) to voice agent. Actual processing of the events would happen in 
 * voice agent context. 
 * This function executes in calling task (server) context.
 */
VAGENT_INTERNAL uint32_t vagent_prepare_and_dispatch_event_packet (
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
)
{

  uint32_t rc = APR_EOK;
  vagent_event_packet_t* packet = NULL;
  vagent_work_item_t* work_item = NULL;

  for ( ;; )
  {
    packet = ( ( vagent_event_packet_t* ) apr_memmgr_malloc( &vagent_heapmgr,
                                           sizeof( vagent_event_packet_t ) ) );
    if ( packet == NULL )
    {
      VAGENT_REPORT_FATAL_ON_NO_MEMORY( APR_ENORESOURCE, sizeof( vagent_event_packet_t ) );
      rc = APR_ENORESOURCE;
      break;
    }

    packet->session_context = session_context;
    packet->event_id = event_id;
    packet->params = NULL;

    if ( ( size > 0 ) && ( params != NULL ) )
    {
      packet->params = apr_memmgr_malloc(  &vagent_heapmgr, size );

      if ( packet->params == NULL )
      {
        rc = APR_ENORESOURCE;
        VAGENT_REPORT_FATAL_ON_NO_MEMORY( rc, size );
        ( void ) vagent_free_event_packet( packet );
        break;
      }
      mmstd_memcpy( packet->params, size, params, size );
    }

    /* Get a free packet structure. */
    rc = apr_list_remove_head( &vagent_free_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) );
    if ( rc )
    {
      rc = APR_ENORESOURCE;
      ( void ) vagent_free_event_packet( packet );
      VAGENT_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    work_item->packet = packet;
    /* Add to incoming request work queue */
    rc = apr_list_add_tail( &vagent_work_pkt_q, &work_item->link );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: Adding to packet queue failed. rc=(0x%08x)", rc );
      /* Add back to vagent_free_work_pkt_q */
      ( void ) vagent_free_event_packet( packet );
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &vagent_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal voice agent thread to run
       */
      vagent_signal_run();
    }
    break;
  } /* for ( ;; ) */

  return rc;

}

/****************************************************************************
 * VAGENT OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES             *
 ****************************************************************************/

VAGENT_INTERNAL uint32_t vagent_mem_alloc_object (
  uint32_t size,
  vagent_object_t** ret_object
)
{

  uint32_t rc = APR_EOK;
  vagent_object_t* vagent_obj;
  apr_objmgr_object_t* objmgr_obj;

  for ( ;; )
  {
    if ( ret_object == NULL )
    {
      rc = APR_EBADPARAM;
      break;
    }

    /* Allocate memory for the VAGENT object. */
    vagent_obj = apr_memmgr_malloc( &vagent_heapmgr, size );
    if ( vagent_obj == NULL )
    {
      rc = APR_ENORESOURCE;
      VAGENT_REPORT_FATAL_ON_NO_MEMORY( rc, size );
      break;
    }

    /* Allocate a new handle for the VAgent object. */
    rc = apr_objmgr_alloc_object( &vagent_objmgr, &objmgr_obj );
    if ( rc )
    {
      rc = APR_ENORESOURCE;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, 
             "VAGENT: ran out of objects, rc=(0x%08x)", rc );
      apr_memmgr_free( &vagent_heapmgr, vagent_obj );
      vagent_obj = NULL;
      break;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = vagent_obj;

    /* Initialize the base MVS object header. */
    vagent_obj->header.handle = objmgr_obj->handle;
    vagent_obj->header.type = VAGENT_OBJECT_TYPE_ENUM_UNINITIALIZED;

    *ret_object = vagent_obj;
    break;
  } /* for ( ;; ) */

  return rc;

}


VAGENT_INTERNAL uint32_t vagent_mem_free_object (
  vagent_object_t* object
)
{
  uint32_t rc = APR_EOK;
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  rc = apr_objmgr_free_object( &vagent_objmgr, object->header.handle );
  VAGENT_REPORT_FATAL_ON_ERROR( rc );
  apr_memmgr_free( &vagent_heapmgr, object );
  object = NULL;

  return rc;

}


VAGENT_INTERNAL uint32_t vagent_create_modem_subs_object ( 
  vagent_modem_subs_object_t** ret_subs_obj
)
{

  uint32_t rc = APR_EOK;
  vagent_modem_subs_object_t* subs_obj = NULL;

  for ( ;; ) 
  {
    if ( ret_subs_obj == NULL )
    {
      rc = APR_EBADPARAM;
      break;
    }

    rc = vagent_mem_alloc_object( sizeof( vagent_modem_subs_object_t ),
                                ( ( vagent_object_t** ) &subs_obj ) );
    if ( rc )
    {
      /* send the error code returned by vagent_mem_alloc_object to
         the caller */
      break;
    }

    /* Initialize the modem subscription object. */
    subs_obj->header.type = VAGENT_OBJECT_TYPE_ENUM_MODEM_SUBSCRIPTION;
    subs_obj->asid = SYS_MODEM_AS_ID_NONE;
    subs_obj->vsid = VAGENT_VSID_UNDEFINED_V;
    subs_obj->session_obj = NULL;

    *ret_subs_obj = subs_obj;
    break;
  } /* for ( ;; ) */

  return rc;

}


VAGENT_INTERNAL uint32_t vagent_create_session_object ( 
  vagent_session_object_t** ret_session_obj
)
{

  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if ( ret_session_obj == NULL )
    {
      rc = APR_EBADPARAM;
      break;
    }

    rc = vagent_mem_alloc_object( sizeof( vagent_session_object_t ),
                                  ( ( vagent_object_t** ) &session_obj ) );
    if ( rc )
    {
      /* send the error code returned by vagent_mem_alloc_object to
         the caller */
      break;
    }

    /* Initialize the session job object. */
    session_obj->header.type = VAGENT_OBJECT_TYPE_ENUM_SESSION;
    session_obj->vsid = VAGENT_VSID_UNDEFINED_V;
    session_obj->modem_subs_obj = NULL;
    session_obj->state = VAGENT_SUBS_STATE_INACTIVE;
    session_obj->is_ring_back_tone_playing = FALSE;
    session_obj->ho_type = SYS_VOICE_HO_NONE;
    session_obj->current_module = VAGENT_MODULE_ID_NONE;
    session_obj->pending_module = VAGENT_MODULE_ID_NONE;
    session_obj->iva_handle = APR_NULL_V;
    session_obj->gva_handle = APR_NULL_V;
    session_obj->wva_handle = APR_NULL_V;
    session_obj->cva_handle = APR_NULL_V;
    session_obj->tva_handle = APR_NULL_V;
    session_obj->ecall_handle = APR_NULL_V;
    session_obj->ecall_resource_request = FALSE;

    *ret_session_obj = session_obj;
    break;
  }

  return rc;

}


VAGENT_INTERNAL void vagent_log_voice_adapter_event_info(
  void* session_context,
  uint32_t event_id
)
{

  vagent_session_object_t* session_obj = NULL;

  if ( session_context == NULL )
  {
    return;
  }

  session_obj = (vagent_session_object_t*)session_context;

  switch( event_id )
  {
   case GVA_IRESOURCE_EVENT_REQUEST:
   {
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "VAGENT: GVA_IRESOURCE_EVENT_REQUEST(CB): ASID=(%d): VSID=(0x%08x):",
            session_obj->modem_subs_obj->asid, session_obj->vsid );
   }
   break;

   case GVA_IRESOURCE_EVENT_RELEASED:
   {
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "VAGENT: GVA_IRESOURCE_EVENT_RELEASED(CB): ASID=(%d): VSID=(0x%08x):",
            session_obj->modem_subs_obj->asid, session_obj->vsid );
   }
   break;

   default:
     break;
  }

  return;
}


/****************************************************************************
 *  VOICE AGENT - CALLBACK FUNCTIONS                                        * 
 *  These function executes in server (CM, voice adapter) context           * 
 *  1. vagent_cm_call_event_cb                                              * 
 *  2. vagent_cm_subs_info_cb                                               * 
 *  3. vagent_voice_adapter_event_cb                                        * 
*****************************************************************************/

VAGENT_INTERNAL void vagent_cm_call_event_cb (
  cm_call_event_e_type call_event,
  const cm_mm_call_info_s_type *call_info_ptr
)
{
   
  uint32_t rc = APR_EOK;
  vagent_modem_subs_object_t* subs_obj = NULL;
  vagent_eventi_cm_call_event_t cm_event_params;

  if ( NULL == call_info_ptr )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vagent_cm_call_event_cb(): call_info_ptr is NULL" );
    return;
  }

  if( ( call_info_ptr->asubs_id < SYS_MODEM_AS_ID_1 )||
      ( call_info_ptr->asubs_id > SYS_MODEM_AS_ID_3 ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vagent_cm_call_event_cb(): ASID=(%d) not supported", 
           call_info_ptr->asubs_id );
    return;
  }

  subs_obj = vagent_modem_subs_obj_list[ call_info_ptr->asubs_id ];
  cm_event_params.asid  = call_info_ptr->asubs_id;
  cm_event_params.event_id = call_event;
  cm_event_params.ho_type = SYS_VOICE_HO_NONE;

  if( ( CM_CALL_TYPE_VOICE == call_info_ptr->call_type ) &&
      ( CM_CALL_TYPE_EMERGENCY == call_info_ptr->call_type ) &&
      ( CM_CALL_TYPE_VT == call_info_ptr->call_type ) )
  {
    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VAGENT: CM_CALL_EVENT(CB): event=(%d): ASID=(%d): VSID=(0x%08x): call_type=(%d), "
           "is_call_type_voice=(%d)", call_event, subs_obj->asid, subs_obj->vsid,
           call_info_ptr->call_type, FALSE );
    return;
  }

  MSG_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VAGENT: CM_CALL_EVENT(CB): event=(%d): ASID=(%d): VSID=(0x%08x): "
         "call_type=(%d) is_call_type_voice=(%d)", call_event, subs_obj->asid,
         subs_obj->vsid, call_info_ptr->call_type, TRUE );

  /* Only following CM events needs to be queued and handled.*/
  switch( call_event )
  {
    case CM_CALL_EVENT_PROGRESS_INFO_IND:
    {
      /* Queue only if inband ring back tones is notified while alerting/progress info indication.
       *
       * This is applicable only to GSM, WCDMA and TDS-CDMA RATs.
       * USER_RESUMED indicates in-band ringback tone.
       */
      if( ( CM_CALL_MODE_INFO_GW_CS != call_info_ptr->mode_info.info_type ) ||
          ( USER_RESUMED != call_info_ptr->mode_info.info.gw_cs_call.call_progress ) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VAGENT: CM_CALL_EVENT(CB): event=(PROGRESS_INFO_IND): ASID=(%d): VSID=(0x%08x): "
               "call_mode=(%d): Ringback tone is NOT in-band: IGNORED ", 
               subs_obj->asid, subs_obj->vsid, call_info_ptr->mode_info.info_type );
        break;
      }

      rc = vagent_prepare_and_dispatch_event_packet( subs_obj,
             VAGENT_EVENTI_CM_CALL_EVENT, &cm_event_params, sizeof( cm_event_params) );
      VAGENT_REPORT_FATAL_ON_ERROR( rc );
    }
    break;

    case CM_CALL_EVENT_ANSWER:
    case CM_CALL_EVENT_CONNECT:
    case CM_CALL_EVENT_END:
    {
      rc = vagent_prepare_and_dispatch_event_packet( subs_obj,
             VAGENT_EVENTI_CM_CALL_EVENT, &cm_event_params, sizeof( cm_event_params) );
      VAGENT_REPORT_FATAL_ON_ERROR( rc );
    }
    break;

    default:
      break;
  } /* end of switch( call_event ) */

  return;
}

VAGENT_INTERNAL void vagent_cm_subs_info_cb (
  cm_ph_event_e_type event,
  const cm_subs_info_s_type* params 
)
{
  uint32_t rc = APR_EOK;
  vagent_modem_subs_object_t* subs_obj = NULL;
  vagent_eventi_cm_subs_info_t vagent_subs_event;

  if ( params == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
         "VAGENT: CM_SUBS_INFO(CB): Invalid event params received." );
    return;
  }

  switch ( event )
  {
     case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
     {
       if ( ( params->subscription_id > SYS_MODEM_AS_ID_NONE ) &&
            ( params->subscription_id < SYS_MODEM_AS_ID_NO_CHANGE ) )
       {
         MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                "VAGENT: CM_SUBS_INFO(CB): VSID=(0x%08x): ASID(%d): event=(%d):"
                "SUB_ACTIVE=(%d)", params->hw_id[SYS_VSID_APP_CS_VOICE], 
                params->subscription_id, event, params->is_subphone_active );
         
         subs_obj = vagent_modem_subs_obj_list[params->subscription_id];
         vagent_subs_event.asid = params->subscription_id;
         vagent_subs_event.vsid = params->hw_id[SYS_VSID_APP_CS_VOICE];
         vagent_subs_event.is_sub_active = params->is_subphone_active;
         rc = vagent_prepare_and_dispatch_event_packet( subs_obj,
                                                        VAGENT_EVENTI_CM_SUBS_INFO, &vagent_subs_event,
                                                        sizeof( vagent_subs_event) );
         VAGENT_REPORT_FATAL_ON_ERROR( rc );
       }
       else
       {
         MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "VAGENT: CM_SUBS_INFO(CB): ASID(%d) not supported: VSID=(0x%08x): "
                "event=(%d)", params->subscription_id,
                params->hw_id[SYS_VSID_APP_CS_VOICE], event );
       }
     }
     break;

     default:
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
              "VAGENT(): CM_SUBS_INFO(CB): Unsupported event=(%d)", event );
     }
     break;
  }

  return;
}

VAGENT_INTERNAL uint32_t vagent_voice_adapter_event_cb (
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
)
{

  uint32_t rc = APR_EOK;

  switch ( event_id )
  {
   case GVA_IRESOURCE_EVENT_REQUEST:
   case GVA_IRESOURCE_EVENT_RELEASED:
   {
     vagent_log_voice_adapter_event_info( session_context, event_id );
     rc = vagent_prepare_and_dispatch_event_packet( session_context,
                                            event_id, params, size );
     VAGENT_REPORT_FATAL_ON_ERROR( rc );
   }
   break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "VAGENT: VOICE_ADAPTER(CB): Unsupported event=(%d)", event_id );
     rc = APR_EFAILED;
     break;
  }

  return rc;
}

/****************************************************************************
 *  End of VOICE AGENT - CALLBACK FUNCTIONS                                 * 
 *  These function executes in server (CM, voice adapter) context           * 
 *  1. vagent_cm_call_event_cb                                              * 
 *  2. vagent_cm_subs_info_cb                                               * 
 *  3. vagent_voice_adapter_event_cb                                        * 
*****************************************************************************/



/****************************************************************************
 *  VOICE AGENT -> VOICE ADAPTER(s) FUNCTIONS                               * 
 *  1. vagent_broadcast_subscription_info                                   * 
 *  2. vagent_resource_register                                             * 
 *  3. vagent_resource_deregister                                           * 
 *  4. vagent_resource_grant                                                * 
 *  5. vagent_resource_revoke                                               * 
*****************************************************************************/

VAGENT_INTERNAL void vagent_broadcast_subscription_info (
  vagent_modem_subs_object_t* subs_obj
)
{

  uint32_t rc = APR_EOK;
  gva_icommon_cmd_set_asid_vsid_mapping_t gva_map_cmd;

  for( ;; )
  {
    /* Send mapping to GVA. */
    gva_map_cmd.asid = subs_obj->asid;
    gva_map_cmd.vsid = subs_obj->vsid;
    rc = gva_call( GVA_ICOMMON_CMD_SET_ASID_VSID_MAPPING, (void*)&gva_map_cmd,
                   sizeof( gva_map_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: ASID-VSID mapping info send(to GVA) failed. rc=(0x%08x)",
             rc );
    }

    if ( APR_EOK == rc )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VAGENT: ASID-VSID mapping info send to voice adapter(s) successfully" );
    }
    break;

  } /* for ( ;; ) */

  return;

}


VAGENT_INTERNAL uint32_t vagent_resource_register (
  vagent_session_object_t* session_obj
)
{

  uint32_t rc = APR_EOK;
  gva_iresource_cmd_register_t gva_reg_cmd;

  for ( ;; )
  {
    /* Register for GVA resource control. */
    gva_reg_cmd.ret_handle = &session_obj->gva_handle;
    gva_reg_cmd.vsid = session_obj->vsid;
    gva_reg_cmd.session_context = ( void* )session_obj;
    gva_reg_cmd.event_cb = vagent_voice_adapter_event_cb;

    rc = gva_call( GVA_IRESOURCE_CMD_REGISTER, (void*)&gva_reg_cmd,
                   sizeof( gva_reg_cmd ) );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: VSID=(0x%08x): ASID=(%d): GVA_IRESOURCE_CMD_REGISTER "
             "failed. rc=(0x%08x) ", session_obj->vsid, 
             session_obj->modem_subs_obj->asid, rc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VAGENT: VSID=(0x%08x): ASID=(%d): GVA_IRESOURCE_CMD_REGISTER "
             "success.", session_obj->vsid, session_obj->modem_subs_obj->asid );
    }

    break;
  }

  return rc;
}


VAGENT_INTERNAL uint32_t vagent_resource_deregister (
  vagent_session_object_t* session_obj
)
{

  uint32_t rc = APR_EOK;
  gva_iresource_cmd_deregister_t gva_dereg_cmd;

  for ( ;; )
  {
    if ( session_obj->gva_handle != APR_NULL_V )
    {
      /* de-Register for GVA resource control. */
      gva_dereg_cmd.handle = session_obj->gva_handle;
  
      rc = gva_call( GVA_IRESOURCE_CMD_DEREGISTER, (void*)&gva_dereg_cmd,
                     sizeof( gva_dereg_cmd ) );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VAGENT: VSID=(0x%08x): ASID=(%d): GVA_IRESOURCE_CMD_DEREGISTER "
               "failed. rc=(0x%08x) ", session_obj->vsid, 
               session_obj->modem_subs_obj->asid, rc );
      }
      else
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VAGENT: VSID=(0x%08x): ASID=(%d): GVA_IRESOURCE_CMD_DEREGISTER "
               "success.", session_obj->vsid, session_obj->modem_subs_obj->asid );
      }
      session_obj->gva_handle =  APR_NULL_V;
    }

    break;
  }

  return rc;
}


VAGENT_INTERNAL uint32_t vagent_resource_grant (
  vagent_session_object_t* session_obj,
  uint32_t module_id
)
{

  uint32_t rc = APR_EOK;
  gva_iresource_cmd_grant_t gva_cmd_grant;

  switch( module_id )
  {
    case VAGENT_MODULE_ID_GVA:
    {
      gva_cmd_grant.handle = session_obj->gva_handle;
      rc = gva_call ( GVA_IRESOURCE_CMD_GRANT, &gva_cmd_grant,
                      sizeof( gva_cmd_grant) );
    }
    break;

    case VAGENT_MODULE_ID_NONE:
    default:
      break;
  }

  return rc;

}


/**  
 * No revoke for ECALL is required. ECALL being the highest 
 * priority usecase hence all other voice use cases are blocked 
 * UE. ECALL would relased the reosurce automatically once the 
 * ECALL is ended. 
 *  
 * NOTE: ECALL only supported on single sim, hence no DSDA use 
 * case with it. 
 */
VAGENT_INTERNAL uint32_t vagent_resource_revoke (
  vagent_session_object_t* session_obj
)
{

  uint32_t rc = APR_EOK;
  gva_iresource_cmd_revoke_t gva_cmd_revoke;
  uint32_t module_id = session_obj->current_module;

  switch( module_id )
  {
    case VAGENT_MODULE_ID_GVA:
    {
      gva_cmd_revoke.handle = session_obj->gva_handle;
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "VAGENT: VSID=(0x%08x): ASID=(%d): GVA_IRESOURCE_CMD_REVOKE issued",
            session_obj->vsid, session_obj->modem_subs_obj->asid  );
      rc = gva_call( GVA_IRESOURCE_CMD_REVOKE, &gva_cmd_revoke,
                     sizeof( gva_cmd_revoke) );
     }
     break;

   case VAGENT_MODULE_ID_NONE:
   default:
     break;
  }

  return rc;

}


VAGENT_INTERNAL void vagent_evaluate_resource_grant (
  vagent_session_object_t* session_obj,
  uint32_t module_id
)
{
  uint32_t rc = APR_EOK;
  
  for ( ;; )
  {
    if ( VAGENT_MODULE_ID_NONE == module_id )
    {
      return;
    }
    if ( ( vagent_num_resources > 0 ) &&
         ( VAGENT_MODULE_ID_NONE == session_obj->current_module ) )
    {
      rc = vagent_resource_grant ( session_obj, module_id );
      if ( rc )
      {
        VAGENT_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED);
        break;
      }

      /* Update the current module to which the resource is granted. */
      session_obj->current_module = module_id;
      /* Clear any stale pending request */
      session_obj->pending_module = VAGENT_MODULE_ID_NONE;
      vagent_num_resources--;

      switch ( module_id )
      {
        case VAGENT_MODULE_ID_GVA:
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "VAGENT: VSID=(0x%08x): ASID=(%d): GVA_IRESOURCE_CMD_GRANT issued."
                 "vagent_num_resources--(%d)", 
                 session_obj->vsid, session_obj->modem_subs_obj->asid, vagent_num_resources );
        }
        break;

        case VAGENT_MODULE_ID_NONE:
        default:
          break;
      }
    }
    else
    {
      if ( session_obj->current_module != VAGENT_MODULE_ID_NONE )
      {
        rc = vagent_resource_revoke( session_obj );
        if ( rc )
        {
          VAGENT_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED);
          break;
        }
        /* Cache the request. */
        session_obj->pending_module = module_id;
      }
    }
    break;
  }
  return;
}

/****************************************************************************
 *  END of VOICE AGENT -> VOICE ADAPTER(s) FUNCTIONS                        * 
 *  1. vagent_broadcast_subscription_info                                   * 
 *  2. vagent_resource_register                                             * 
 *  3. vagent_resource_deregister                                           * 
 *  4. vagent_resource_grant                                                * 
 *  5. vagent_resource_revoke                                               * 
*****************************************************************************/



/****************************************************************************
 *  VOICE AGENT - EVENT CALLBACK PROCESSING FUNCTIONS                       * 
 *  These function executes in voice agent context                          * 
 *  1. vagent_process_cm_call_event                                         * 
 *  2. vagent_process_cm_subs_info_event                                    * 
 *  3. vagent_process_resource_request_event                                * 
 *  4. vagent_process_ecall_resource_request_event                          * 
 *  5. vagent_process_resource_released_event                               * 
 *  6. vagent_process_ecall_resource_released_event                         * 
*****************************************************************************/

VAGENT_INTERNAL uint32_t vagent_process_cm_subs_info_event ( 
  vagent_event_packet_t* packet
)
{

  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  vagent_modem_subs_object_t* subs_obj = NULL;
  vagent_session_object_t* session_obj = NULL;
  vagent_eventi_cm_subs_info_t* evt_params = NULL;

  for ( ;; )
  {
    evt_params = ( vagent_eventi_cm_subs_info_t*) packet->params;
    if( evt_params == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    subs_obj = vagent_modem_subs_obj_list[evt_params->asid];

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VAGENT: CM_SUBS_INFO(PROCESS): ASID=(%d), new_VSID=(0x%08x),"
           "current_VSID=(0x%08x)", evt_params->asid, evt_params->vsid, 
           subs_obj->vsid );

    /* This condition would be TRUE when there is no change in
     * ASID-VSID mapping (i.e post boot-up) 
     * No action required - just return from this function.
     */
    if( subs_obj->vsid == evt_params->vsid )
    {
      break;
    }

    /* Find session object with vsid.
     * Assign a session object if it does not exist. */
    for ( index = 0; index < VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V; ++index )
    {
      session_obj = vagent_session_obj_list[index];
      /* This condition would be TRUE when ASID-VSID mapping is received
       * for the first time (boot-up time) 
       */
      if( session_obj->vsid == VAGENT_VSID_UNDEFINED_V )
      {
        session_obj->vsid = evt_params->vsid;
        subs_obj->vsid = session_obj->vsid;
        subs_obj->session_obj = session_obj;
        session_obj->modem_subs_obj = subs_obj;
        ( void ) vagent_resource_register( session_obj );
        /** Initiate voice logging sessiosn with VSID*/
        ( void ) voicelog_session_init( session_obj->vsid );
        break;
      }
      /* This condition would be TRUE when there is a change in
       * ASID-VSID mapping.
       */
      if( session_obj->vsid == evt_params->vsid )
      {
        subs_obj->vsid = session_obj->vsid;
        subs_obj->session_obj = session_obj;
        session_obj->modem_subs_obj = subs_obj;
        break;
      }
    }
    vagent_broadcast_subscription_info( subs_obj );
    break;
  }

  ( void ) vagent_free_event_packet( packet );

  return rc;
}

VAGENT_INTERNAL uint32_t vagent_process_cm_call_event ( 
  vagent_event_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vagent_modem_subs_object_t* subs_obj = NULL;
  vagent_session_object_t* session_obj = NULL;
  vagent_eventi_cm_call_event_t* evt_params = NULL;

  for ( ;; )
  {
    evt_params = ( vagent_eventi_cm_call_event_t*) packet->params;
    if ( NULL == evt_params )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    subs_obj = vagent_modem_subs_obj_list[evt_params->asid];
    if ( NULL == subs_obj )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    /* Extract the mapped VSID session object. */
    session_obj = subs_obj->session_obj;
    if ( NULL == session_obj )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VAGENT: CM_CALL_EVENT(PROCESS): event=(%d): VSID=(0x%08x): ASID=(%d): "
		   "Vagent_state=(%d): no_of_resources=(%d) ", evt_params->event_id,
		   subs_obj->vsid, subs_obj->asid, session_obj->state, vagent_num_resources );

    /* Check for the overall call status on subscription.
     * If Connected:
     *     - Move the voice agent state to active.
     *     - Grant for pending request.
     */
    switch( evt_params->event_id )
    {
      case CM_CALL_EVENT_ANSWER:
      case CM_CALL_EVENT_PROGRESS_INFO_IND:
      {
        /* The following checks help to properly update call number only once even 
         * when CM sends multiple events for the below listed scenarios:
         * 1. CM sends Multiple CM_CALL_EVENT_PROGRESS_INFO_IND event for the same MO call when the user doesn't accepts the call.
         * 2. CM sends Multiple call answer event for Multi party/ Multi call scenarios. 
              But with respect to Voice resource the call number is kept same. */
        if( ( FALSE == session_obj->is_ring_back_tone_playing ) && 
            ( VAGENT_SUBS_STATE_INACTIVE == session_obj->state ) )
        {
          ( void ) voicelog_session_update( session_obj->vsid, VOICELOG_INFO_ENUM_CALL_NUMBER );
        }

        session_obj->is_ring_back_tone_playing = TRUE;/* Replace with is_early_acquisition. */
        session_obj->state = VAGENT_SUBS_STATE_ACTIVE;

        vagent_evaluate_resource_grant( session_obj, session_obj->pending_module );
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "VAGENT: CM_CALL_EVENT(PROCESS): Vagent_state(%d) Early vocoder aquisition",
			   session_obj->state );
      }
      break;

      case CM_CALL_EVENT_CONNECT:
      {
        /* Update call number here only for MO calls without early ringback tone.
         * 
         * The call number will be updated earlier in the below scenarios, So no need to update again.
         * 1. In case of MO calls with ringback tones the call number will be updated for CM_CALL_EVENT_PROGRESS_INFO_IND.
         * 2. In case of MT calls "CM_CALL_EVENT_ANSWER" event is received and then CM_CALL_EVENT_CONNECT event is received. 
              The call number will be updated for CM_CALL_EVENT_ANSWER event. */
        if( VAGENT_SUBS_STATE_INACTIVE == session_obj->state )
        {
          ( void ) voicelog_session_update( session_obj->vsid, VOICELOG_INFO_ENUM_CALL_NUMBER );
        }

        /* Clear is_ring_back_tone_playing flag as call is CONNECTED. */
        session_obj->is_ring_back_tone_playing = FALSE;
        session_obj->state = VAGENT_SUBS_STATE_ACTIVE;
        vagent_evaluate_resource_grant( session_obj, session_obj->pending_module );
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "VAGENT: CM_CALL_EVENT(PROCESS): Vagent_state(%d) is_ring_back_tone_playing(%d)", 
               session_obj->state, session_obj->is_ring_back_tone_playing );
      }
      break;

      case CM_CALL_EVENT_END:
      {
        /* For 9x05 mutiple CALL END event is not expected. */
        if ( VAGENT_SUBS_STATE_INACTIVE == session_obj->state )
        VAGENT_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );

        session_obj->ho_type = SYS_VOICE_HO_NONE;
        session_obj->state = VAGENT_SUBS_STATE_INACTIVE;
        session_obj->is_ring_back_tone_playing = FALSE;

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "VAGENT: CM_CALL_EVENT(PROCESS):CALL_END vagent_state(%d) is_ring_back_tone_playing(%d)", 
               session_obj->state, session_obj->is_ring_back_tone_playing );
      }
      break;

      default:
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VAGENT: CM_CALL_EVENT(PROCESS): Vagent_state(%d)Unhandled cm_event(%d) ",
               session_obj->state, evt_params->event_id );
        break;
    }

    break;
  }

  ( void ) vagent_free_event_packet( packet );

  return rc;
}

/************************************************************************
 ADAPTER EVENT HANDLER                                                   
 ************************************************************************/
VAGENT_INTERNAL uint32_t vagent_process_resource_request_event ( 
  vagent_event_packet_t* packet,
  uint32_t module_id
)
{
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_modem_subs_object_t* subs_obj = NULL;

  for ( ;; )
  {
    session_obj = packet->session_context;
    if( session_obj == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    subs_obj = session_obj->modem_subs_obj;
    if ( subs_obj == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    if ( session_obj->current_module == module_id )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "VAGENT: IRESOURCE_EVENT_REQUEST(PROCESS): resource already granted "
            "to module_id(%d): VSID=(0x%08x): ASID=(%d): Vagent_state=(%d)", 
             module_id, subs_obj->vsid, subs_obj->asid, session_obj->state );
      break;
    }

    /* Process the request from CS Adapters only if in ACTIVE state or if RBT
     * is in-progress else cache the request in pending_module.
     */
    if ( ( VAGENT_SUBS_STATE_ACTIVE == session_obj->state ) ||
         ( TRUE == session_obj->is_ring_back_tone_playing ) )
    {
      vagent_evaluate_resource_grant( session_obj, module_id );
    }
    else
    {
      /* Cache the resource in pending module. */
      session_obj->pending_module = module_id;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "VAGENT: IRESOURCE_EVENT_REQUEST(PROCESS): Caching the request for "
             "module_id(%d)", module_id );
    } 

    break;
  }

  ( void ) vagent_free_event_packet( packet );
  return rc;
}

VAGENT_INTERNAL uint32_t vagent_process_resource_released_event ( 
  vagent_event_packet_t* packet,
  uint32_t module_id
)
{
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_modem_subs_object_t* subs_obj = NULL;

  for ( ;; )
  {
    session_obj = packet->session_context;
    if( session_obj == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    subs_obj = session_obj->modem_subs_obj;
    if( subs_obj == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    if ( session_obj->pending_module == module_id )
    {
      /* Clearing cached request(if any). */
      session_obj->pending_module = VAGENT_MODULE_ID_NONE;

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: IRESOURCE_EVENT_RELEASE(PROCESS): release event from "
             "module(%d) when it is NOT granted previously", module_id );
      break;
    }

    if ( session_obj->current_module !=  module_id )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: IRESOURCE_EVENT_RELEASE(PROCESS): VSID:(0x%08x) release "
             "event from different module=(%d): current_module=(%d), pending_module=(%d)",
             session_obj->vsid, module_id, session_obj->current_module, session_obj->pending_module );
      break;
    }

    vagent_num_resources++;
    session_obj->current_module = VAGENT_MODULE_ID_NONE;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VAGENT: IRESOURCE_EVENT_RELEASE(PROCESS): vagent_num_resources++(%d)",
            vagent_num_resources );

    if ( ( TRUE == session_obj->is_ring_back_tone_playing ) ||
         ( VAGENT_SUBS_STATE_ACTIVE == session_obj->state ) ||
         ( VAGENT_MODULE_ID_IVA == session_obj->pending_module ) )
    {
      vagent_evaluate_resource_grant( session_obj, session_obj->pending_module );
    }

    break;
  }

  ( void ) vagent_free_event_packet( packet );
  return rc;
}

VAGENT_INTERNAL void vagent_process_event_packets (
  void
)
{
  uint32_t request_id = 0;
  vagent_work_item_t* work_item = NULL;
  vagent_event_packet_t* packet = NULL;

  while( apr_list_remove_head( &vagent_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;

    /* Add back to vs_free_work_pkt_q. */
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &vagent_free_work_pkt_q, &work_item->link );

    if ( packet == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vagent_process_event_packets(): work packet NULL!! " );
      continue;
    }
    else
    {
      request_id = packet->event_id;
    }

    switch( request_id )
    {
      /**
       * Handling routine for nongating work-items should take of release the
       * memory allocated for the CMD/EVENT packets.
       */
      case GVA_IRESOURCE_EVENT_REQUEST:
        ( void ) vagent_process_resource_request_event( packet, VAGENT_MODULE_ID_GVA );
        break;
 
      case GVA_IRESOURCE_EVENT_RELEASED:
        ( void ) vagent_process_resource_released_event( packet, VAGENT_MODULE_ID_GVA );
        break;

      case VAGENT_EVENTI_CM_SUBS_INFO:
       ( void ) vagent_process_cm_subs_info_event( packet );
       break;
 
      case VAGENT_EVENTI_CM_CALL_EVENT:
        ( void ) vagent_process_cm_call_event ( packet );
        break;

      default:
        /* Handle unsupported request. */
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VAGENT: Unsupported event=(%d)", request_id );
        vagent_free_event_packet( packet );
        break;
    } /* switch case ends. */
  } /* while loop ends. */

  return;
}

/****************************************************************************
 *  END of VOICE AGENT - EVENT CALLBACK PROCESSING FUNCTIONS                * 
 *  These function executes in voice agent context                          * 
 *  1. vagent_process_cm_call_event                                         * 
 *  2. vagent_process_cm_subs_info_event                                    * 
 *  3. vagent_process_resource_request_event                                * 
 *  4. vagent_process_ecall_resource_request_event                          * 
 *  5. vagent_process_resource_released_event                               * 
 *  6. vagent_process_ecall_resource_released_event                         * 
 *  7. vagent_process_nongating_work_items                                  * 
*****************************************************************************/



/****************************************************************************
 * TASK ROUTINES                                                        *
 ****************************************************************************/

/* This function executes in calling task (server) context */
VAGENT_INTERNAL void vagent_signal_run (
  void
)
{
  apr_event_signal( vagent_work_event );
}


VAGENT_INTERNAL int32_t vagent_run (
  void
)
{
  vagent_process_event_packets( );
  return APR_EOK;
}

/* Voice agent's main worker thread function */
VAGENT_INTERNAL int32_t vagent_worker_thread_fn (
  void* param
)
{
  int32_t rc = APR_EOK;

  apr_event_create( &vagent_work_event );
  apr_event_signal( vagent_thread_create_event );

  for ( ;; )
  {
    rc = apr_event_wait( vagent_work_event );
    if ( rc )
    {
      break;
    }
    vagent_run( );
  }

  apr_event_destroy( vagent_work_event );
  apr_event_signal( vagent_thread_create_event );

  return rc;
}

/****************************************************************************
 * POWER UP/DOWN ROUTINES                                                   *
 ****************************************************************************/

VAGENT_INTERNAL int32_t vagent_init (
  void 
)
{

  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  RCINIT_INFO info_handle = NULL;
  RCINIT_PRIO priority = 0;
  unsigned long stack_size = 0;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== vagent_init(): ======" );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &vagent_int_lock );
    apr_event_create( &vagent_thread_create_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &vagent_heapmgr, ( ( void* ) &vagent_heap_pool ),
                          sizeof( vagent_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = vagent_object_table;
    params.total_bits = VAGENT_HANDLE_TOTAL_BITS_V;
    params.index_bits = VAGENT_HANDLE_INDEX_BITS_V;
    params.lock_fn = vagent_int_lock_fn;
    params.unlock_fn = vagent_int_unlock_fn;
    rc = apr_objmgr_construct( &vagent_objmgr, &params );
    VAGENT_PANIC_ON_ERROR( rc );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &vagent_free_work_pkt_q, 
                           vagent_int_lock_fn, vagent_int_unlock_fn );
    for ( index = 0; index < VAGENT_NUM_WORK_PKTS_V; ++index )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &vagent_work_pkts[index] );
      vagent_work_pkts[index].packet = NULL;
      rc = apr_list_add_tail( &vagent_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &vagent_work_pkts[index] ) );
    }

    rc = apr_list_init_v2( &vagent_work_pkt_q,
                           vagent_int_lock_fn, vagent_int_unlock_fn );
  }

  { /* Create the GVA task worker thread. */
#if ( !defined AVS_MPSS_TEST )
    info_handle = rcinit_lookup( VAGENT_TASK_NAME );
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
           "vagent_init(): VAGENT task not registered with RCINIT" );
      priority = VAGENT_TASK_PRIORITY;
      stack_size = VAGENT_TASK_STACK_SIZE;
    }

    if ( ( priority > 255 ) || ( stack_size == 0 ) ) 
    {
      ERR_FATAL( "vagent_init(): Invalid priority: %d or stack size: %d",
                 priority, stack_size, 0 );
    }

    rc = apr_thread_create( &vagent_thread, VAGENT_TASK_NAME, TASK_PRIORITY( priority ),
                            vagent_task_stack, stack_size, 
                            vagent_worker_thread_fn , NULL );
    VAGENT_PANIC_ON_ERROR( rc );

    apr_event_wait( vagent_thread_create_event );
  }

  vagent_is_initialized = TRUE;

  return rc;

}


VAGENT_INTERNAL int32_t vagent_postinit ( 
  void
)
{

  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  bool_t ret_val = FALSE;
  cm_client_id_type vagent_client_id = CM_CLIENT_TYPE_NONE;
  bit_mask_256_t event_mask;
  vagent_modem_subs_object_t* subs_obj = NULL;
  vagent_eventi_cm_subs_info_t vagent_subs_event;

  /* Initialize the subscription/session object list. */
  for( index = 0; index < VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V; ++index )
  {
    rc = vagent_create_modem_subs_object( &vagent_modem_subs_obj_list[index] );
    VAGENT_PANIC_ON_ERROR(rc);

    vagent_modem_subs_obj_list[index]->asid = ( sys_modem_as_id_e_type )index;
    rc = vagent_create_session_object( &vagent_session_obj_list[index] );
    VAGENT_PANIC_ON_ERROR(rc);

  }

  /* CM registration for CM Phone event for asid->vsid mapping. */
  rc = cm_client_init( CM_CLIENT_TYPE_VA, &vagent_client_id );
  if ( rc == CM_CLIENT_OK )
  {
    /* Resigter with CM for subscription information events. This event is
     * generated when subscription information is changed. 
     * Voice Agent is interested in knowing when subscription available/active
     * status changes, and ASID-VSID mapping information changes.
     */
    cm_client_subs_reg( vagent_client_id, vagent_cm_subs_info_cb );

    bit_mask_256_clear_all(&event_mask);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_ANSWER);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_PROGRESS_INFO_IND);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_CONNECT);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_HO_START);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_HO_COMPLETE);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_END_REQ);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_END);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_SUPS);
    bit_mask_256_set_bit(&event_mask,CM_CALL_EVENT_INCOM);

    /* Regsiter for CM call events */
    cm_mm_client_call_reg( vagent_client_id, vagent_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG, 
                           event_mask, NULL );

  }
  else
  {
    VAGENT_REPORT_FATAL_ON_ERROR( rc );
  }

  /* TODO: CM regitration for LCH/ACTIVE events per subscription for DSDA. */

  /* Required, if CM has already broadcasted CM phone event before vagent
   * boot-up initialization. */
  index = 0;
  while ( index < VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V )
  {
    /* CM will provide the mapping through the CB API registered
     * via cm_client_subs_reg().
     */
    ret_val = cm_ph_cmd_get_subs_pref_info( NULL, NULL, vagent_client_id,
                                            ( sys_modem_as_id_e_type ) index );
    if ( ret_val == FALSE ) 
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_ENORESOURCE );
    }
    index++;
  }
  
 
  /*workaround for default VSID-ASID mapping.
   *This is a temporary change, to be removed when actual 
   *solution is pulled in. */
  subs_obj = vagent_modem_subs_obj_list[0];
  vagent_subs_event.asid = 0;
  vagent_subs_event.vsid = 0x11C05000;
  vagent_subs_event.is_sub_active = 1;

  rc = vagent_prepare_and_dispatch_event_packet( subs_obj,
                                                 VAGENT_EVENTI_CM_SUBS_INFO, &vagent_subs_event,
                                                  sizeof( vagent_subs_event) );

  return rc;
}


VAGENT_INTERNAL int32_t vagent_predeinit (
  void
)
{

  uint32_t rc = APR_EOK;
  uint32_t index = 0;

  /* Destroy Session/Subscription object list. */
  for( index = 0; index < VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V; ++index )
  {
    ( void )vagent_mem_free_object( 
                   ( vagent_object_t* )vagent_modem_subs_obj_list[index] );

   ( void) vagent_resource_deregister( vagent_session_obj_list[index] );

    ( void )vagent_mem_free_object(
                   ( vagent_object_t* )vagent_session_obj_list[index] );
  }

  return rc;

}


VAGENT_INTERNAL int32_t vagent_deinit (
  void 
)
{
  uint32_t rc = APR_EOK;

  vagent_is_initialized = FALSE;

  apr_event_signal_abortall( vagent_work_event );
  apr_event_wait( vagent_thread_create_event );

  /* Release work queue */
  ( void ) apr_list_destroy( &vagent_free_work_pkt_q );
  ( void ) apr_list_destroy( &vagent_work_pkt_q );

  /* Deinitialize the object handle table. */
  ( void ) apr_objmgr_destruct( &vagent_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  ( void ) apr_event_destroy( vagent_thread_create_event );
  ( void ) apr_lock_destroy( vagent_int_lock );

  return rc;
}


/****************************************************************************
 * SINGLE ENTRY POINT                                                       *
 ****************************************************************************/

VAGENT_EXTERNAL uint32_t vagent_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;

  if ( ( FALSE == vagent_is_initialized ) && ( DRV_CMDID_INIT != cmd_id ) )
  {
    VAGENT_REPORT_FATAL_ON_ERROR( APR_ENOSERVICE );
    return APR_ENOSERVICE;
  }

  switch ( cmd_id )
  {
    case DRV_CMDID_INIT:
      rc = vagent_init( );
      break;
  
    case DRV_CMDID_POSTINIT:
      rc = vagent_postinit( );
      break;
  
    case DRV_CMDID_PREDEINIT:
      rc = vagent_predeinit( );
      break;
  
    case DRV_CMDID_DEINIT:
      rc = vagent_deinit( );
      break;
  
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vagent_call(): Unsupported cmd ID (0x%08x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;

}
