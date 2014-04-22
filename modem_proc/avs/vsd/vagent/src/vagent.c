/*
   Copyright (C) 2015-2019 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vagent/src/vagent.c#1 $
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

/* VSD APIs. */
#include "drv_api.h"
#include "vs_task.h"
#include "voicelog_utils.h"

/* SELF APIs. */
#include "vagent.h"
#include "vagent_i.h"


/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Size of VS work packet queue. */
#define VAGENT_NUM_WORK_PKTS_V ( 10 )

/* Currently voice agent supports three subscriptions.
 * One VSID per subscription */
#define VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V ( 3 )

/* Represent that number of voice sessions(CVD) that can exist concurrently 
 * at a time in the system. */
#define VAGENT_MAX_NUM_OF_RESOURCES_V ( 1 )

/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

/**< VS service lock to prevent concurrent client access. */
static apr_lock_t vagent_int_lock;
static apr_event_t vagent_thread_create_event;

static apr_list_t vagent_work_pkt_q;
static apr_list_t vagent_free_work_pkt_q;
static vagent_work_item_t vagent_work_pkts[ VAGENT_NUM_WORK_PKTS_V ];

static apr_event_t vagent_work_event;
static apr_thread_t vagent_thread;
static uint8_t vagent_task_stack[ VAGENT_TASK_STACK_SIZE ];

static vagent_session_object_t vagent_session_obj_list[VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V];

static bool_t vagent_is_initialized = FALSE;

static uint32_t vagent_num_resources = VAGENT_MAX_NUM_OF_RESOURCES_V;
  /**< Indicates total number of voice resource available for grant. */

/* GVA address is set at initialization. */
static char_t vagent_my_dns[] =  "qcom.audio.vagent";
static uint16_t vagent_my_addr;
static uint32_t vagent_apr_handle;

/* Default VSIDs used by voice agent. */
static uint32_t vagent_vsid_table[] = {
  0x11C05000,
  0x11DC5000
};

/* Client callback registration table. */
static vagent_ivoice_cmd_register_t vagent_client_registration_table[VAGENT_VOICE_CLIENT_MAX];

static char* vagent_client_enum_table[VAGENT_VOICE_CLIENT_MAX] = 
{
  "CVA",
  "GVA",
  "WVA",
  "TVA",
  "IMS",
  "ECALL",
  "QCHAT"
};

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void vagent_int_lock_fn ( void )
{
  apr_lock_enter( vagent_int_lock );
}

static void vagent_int_unlock_fn ( void )
{
  apr_lock_leave( vagent_int_lock );
}


static int32_t vagent_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t rc;
  rc = __aprv2_cmd_free( vagent_apr_handle, packet );
  VAGENT_REPORT_FATAL_ON_ERROR(rc);

  return APR_EOK;
}

static void vagent_signal_run ( void )
{
  apr_event_signal( vagent_work_event );
}


/****************************************************************************
 * CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                      *
*****************************************************************************/

/**
 * Queues the aprv2_packet_t (queues both incoming and pending packets to 
 * corresponding queues). In case of failure to queue a apr packet, packet 
 * will be automatically freed.
 */
static uint32_t vagent_queue_apr_packet (
  aprv2_packet_t* packet,
  apr_list_t* cmd_q
)
{
  uint32_t rc = APR_EOK;
  vagent_work_item_t* work_item = NULL;
  
  if( packet == NULL )
  {
    VAGENT_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    return APR_EFAILED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &vagent_free_work_pkt_q, (apr_list_node_t**)&work_item );
    if ( NULL == work_item )
    {
      VAGENT_REPORT_FATAL_ON_ERROR(APR_ENORESOURCE);
      break;
    }

    work_item->packet = packet;
    rc = apr_list_add_tail( cmd_q, &work_item->link );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: error in queuing APR packet: opcode[0x%08x], rc[0x%08x]",
             packet->opcode, rc );
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &vagent_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /* Signal appropriate thread. */
      vagent_signal_run( );
    }

    break;
  } /* for loop ends. */

  return rc;
}

static uint32_t vagent_prepare_and_dispatch_cmd_packet (
  void* session_context,
  uint32_t cmd_id,
  void* cmd_params,
  uint32_t params_size
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = NULL;
  void* payload;

  for ( ;; )
  {
    rc = __aprv2_cmd_alloc_ext( vagent_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                APR_NULL_V, APR_NULL_V,
                                APR_NULL_V, APR_NULL_V, 
                                (uint32_t)session_context, cmd_id,
                                params_size, &packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: APR packet allocation failed with rc=[0x%08x]", rc );
      break;
    }

    if ( ( params_size > 0 ) && ( cmd_params != NULL ) )
    {
      payload = APRV2_PKT_GET_PAYLOAD( void, packet);
      mmstd_memcpy( payload, params_size, cmd_params, params_size );
    }

    /* Queue the command packet for processing. */
    rc = vagent_queue_apr_packet( packet, &vagent_work_pkt_q );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VAGENT: APR packet queuing failed. rc[0x%08x]", rc );
      rc = __aprv2_cmd_free( vagent_apr_handle, packet );
      VAGENT_REPORT_FATAL_ON_ERROR(rc);
    }

    break;
  }

  return rc;
}


static bool_t vagent_is_valid_ho_type( sys_voice_handover_e_type ho_type )
{
  if ( ( SYS_VOICE_HO_G_2_G == ho_type ) ||
       ( SYS_VOICE_HO_G_2_W == ho_type ) ||
       ( SYS_VOICE_HO_W_2_W == ho_type ) ||
       ( SYS_VOICE_HO_W_2_G == ho_type ) ||
       ( SYS_VOICE_HO_SRVCC_L_2_G == ho_type ) ||
       ( SYS_VOICE_HO_SRVCC_L_2_W == ho_type ) ||
       ( SYS_VOICE_HO_SRVCC_L_2_C == ho_type ) || 
       ( SYS_VOICE_HO_DRVCC_WIFI_2_C == ho_type ) || 
       ( SYS_VOICE_HO_DRVCC_WIFI_2_GW == ho_type ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/****************************************************************************
 *  VOICE AGENT - CALLBACK FUNCTIONS                                        * 
 *  These function executes in server (CM, voice adapter) context           * 
 *  1. vagent_cm_call_event_cb                                              * 
 *  2. vagent_cm_subs_info_cb                                               * 
*****************************************************************************/

static void vagent_cm_call_event_cb (
  cm_call_event_e_type call_event,
  const cm_mm_call_info_s_type *call_info_ptr
)
{
   
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_eventi_cm_call_event_t cm_event_params;

  if ( NULL == call_info_ptr )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VAGENT: CM_CALL_EVENT_CB: call_info_ptr is NULL" );
    return;
  }

  if( ( call_info_ptr->asubs_id < SYS_MODEM_AS_ID_1 )||
      ( call_info_ptr->asubs_id > SYS_MODEM_AS_ID_3 ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VAGENT: CM_CALL_EVENT_CB: ASID=(%d) not supported", 
           call_info_ptr->asubs_id );
    return;
  }

  session_obj = &vagent_session_obj_list[ call_info_ptr->asubs_id ];
  cm_event_params.event_id = call_event;
  cm_event_params.asid  = call_info_ptr->asubs_id;
  cm_event_params.call_state = call_info_ptr->connected_call_status_per_sub;
  cm_event_params.ho_type = SYS_VOICE_HO_NONE;

  /* Only following CM events needs to be queued and handled.*/
  switch( call_event )
  {
    case CM_CALL_EVENT_ANSWER:
    case CM_CALL_EVENT_CONNECT:
    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_PROGRESS_INFO_IND:
    {
      if( TRUE == call_info_ptr->vocoder_srv )
      {
        /* Queue only if inband ring back tones is notified while 
         * alerting/progress info indication.
         * This is applicable only to GSM, WCDMA and TDS-CDMA RATs. 
         * USER_RESUMED indicates in-band ringback tone.
         */
        if( ( CM_CALL_EVENT_PROGRESS_INFO_IND == call_event ) &&
            ( ( CM_CALL_MODE_INFO_GW_CS != call_info_ptr->mode_info.info_type ) ||
              ( USER_RESUMED != call_info_ptr->mode_info.info.gw_cs_call.call_progress ) ) )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "VAGENT: CM_CALL_EVENT(CB): event=(PROGRESS_INFO_IND): ASID=(%d): VSID=(0x%08x): "
                 "call_mode=(%d): Ringback tone is NOT in-band: IGNORED ", 
                 session_obj->asid, session_obj->vsid, call_info_ptr->mode_info.info_type );
          break;
        }

        MSG_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VAGENT: CM_CALL_EVENT(CB): event=(%d): ASID=(%d): VSID=(0x%08x): "
               "call_type=(%d) is_call_type_voice=(%d)", call_event, session_obj->asid,
               session_obj->vsid, call_info_ptr->call_type, call_info_ptr->vocoder_srv );

        rc = vagent_prepare_and_dispatch_cmd_packet( session_obj,
                VAGENT_EVENTI_CM_CALL_EVENT, &cm_event_params, sizeof( cm_event_params) );
        VAGENT_REPORT_FATAL_ON_ERROR( rc );
      }
    }
    break;

    /* call_type is not applicable for handover event and during data call 
     * handover events are not expected from call manager. */
    case CM_CALL_EVENT_HO_START:
    case CM_CALL_EVENT_HO_COMPLETE:
    {
      cm_event_params.alert_media_type = call_info_ptr->alert_media_type;
      cm_event_params.ho_type = call_info_ptr->voice_ho_type;

      if ( vagent_is_valid_ho_type( call_info_ptr->voice_ho_type ) )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VAGENT: CM_CALL_EVENT(CB): event=(%d): ASID=(%d): VSID=(0x%08x): "
               "HO_type=(%d)", call_event, session_obj->asid, session_obj->vsid, 
               call_info_ptr->voice_ho_type );
        rc = vagent_prepare_and_dispatch_cmd_packet( session_obj,
                VAGENT_EVENTI_CM_CALL_EVENT, &cm_event_params, sizeof( cm_event_params) );
        VAGENT_REPORT_FATAL_ON_ERROR( rc );
      }
      else
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VAGENT: CM_CALL_EVENT(CB): event=(%d): ASID=(%d): VSID=(0x%08x): "
               "Unhandled handover type=(%d)", call_event, session_obj->asid,
               session_obj->vsid, call_info_ptr->voice_ho_type );
      }
    }
    break;

    default:
      break;

  } /* end of switch( call_event ) */

  return;
}


static void vagent_cm_subs_info_cb (
  cm_ph_event_e_type event,
  const cm_subs_info_s_type* params 
)
{

  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
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
         
         session_obj = &vagent_session_obj_list[params->subscription_id];
         vagent_subs_event.asid = params->subscription_id;
         vagent_subs_event.vsid = params->hw_id[SYS_VSID_APP_CS_VOICE];
         vagent_subs_event.is_sub_active = params->is_subphone_active;
         rc = vagent_prepare_and_dispatch_cmd_packet( session_obj,
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



/****************************************************************************
 *  VOICE AGENT -> VOICE ADAPTER(s) FUNCTIONS                               * 
 *  1. vagent_resource_grant                                                * 
 *  2. vagent_resource_revoke                                               * 
*****************************************************************************/

static uint32_t vagent_resource_grant (
  vagent_session_object_t* session_obj,
  vagent_voice_client_type_t module_id
)
{
  uint32_t rc = APR_EOK;
  vagent_ivoice_cmd_register_t *registration_entry = NULL;
  vagent_ivoice_event_resource_grant_t grant_event;

  registration_entry = &vagent_client_registration_table[module_id];
  grant_event.asid = session_obj->asid;
  grant_event.vsid = session_obj->vsid;

  if ( NULL != registration_entry->client_cb )
  {
    rc = registration_entry->client_cb( VAGENT_IVOICE_EVENT_RESOURCE_GRANT,
           (void*)&grant_event, sizeof(grant_event), registration_entry->client_data );

    MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VAGENT: ASID:[%d] VSID:[0x%08x] granting resource to client[%s]",
                   session_obj->asid, session_obj->vsid, vagent_client_enum_table[module_id] );
  }
  else
  {
    MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VAGENT: ASID:[%d] VSID:[0x%08x] registration missing for client[%s] ",
                   session_obj->asid, session_obj->vsid, vagent_client_enum_table[module_id] );
  }

  return APR_EOK;
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
 uint32_t vagent_resource_revoke (
  vagent_session_object_t* session_obj,
  vagent_voice_client_type_t module_id
)
{
  uint32_t rc = APR_EOK;
  vagent_ivoice_cmd_register_t *registration_entry = NULL;
  vagent_ivoice_event_resource_revoke_t revoke_event;

  registration_entry = &vagent_client_registration_table[module_id];
  revoke_event.asid = session_obj->asid;

  if ( NULL != registration_entry->client_cb )
  {
    rc = registration_entry->client_cb( VAGENT_IVOICE_EVENT_RESOURCE_REVOKE,
           (void*)&revoke_event, sizeof(revoke_event), registration_entry->client_data );

    MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VAGENT: ASID[%d]: VSID[0x%08x]: revoking resource from client[%s]",
                   session_obj->asid, session_obj->vsid, vagent_client_enum_table[module_id] );
  }
  else
  {
    MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VAGENT: ASID:[%d] VSID:[0x%08x] registration missing for client[%s] ",
                   session_obj->asid, session_obj->vsid, vagent_client_enum_table[module_id] );
  }

  return APR_EOK;
}


static void vagent_evaluate_resource_grant (
  vagent_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;

  for ( ;; )
  {
    if ( VAGENT_VOICE_CLIENT_INVALID == session_obj->pending_module )
    {
      return;
    }

    if ( ( vagent_num_resources > 0 ) &&
         ( VAGENT_VOICE_CLIENT_INVALID == session_obj->current_module ) )
    {
      rc = vagent_resource_grant ( session_obj, session_obj->pending_module );

      /* Update the current module to which the resource is granted. */
      session_obj->current_module = session_obj->pending_module;
      session_obj->pending_module = VAGENT_VOICE_CLIENT_INVALID;
      vagent_num_resources--;

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VAGENT: available vagent_num_resources[%d]", vagent_num_resources );
      break;
    }

    if ( VAGENT_VOICE_CLIENT_INVALID != session_obj->current_module )
    {
      rc = vagent_resource_revoke( session_obj, session_obj->current_module );
      break;
    }

    break;
  }

  return;
}


/****************************************************************************
 *  VAGENT COMMAND/EVENT POCESSING FUNCTIONS                             * 
 *  These function executes in voice agent context                          * 
 *****************************************************************************/

static uint32_t vagent_process_cm_subs_info_event ( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_eventi_cm_subs_info_t* evt_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vagent_eventi_cm_subs_info_t, packet );
    if( evt_params == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    session_obj = &vagent_session_obj_list[evt_params->asid];

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VAGENT: CM_SUBS_INFO(PROCESS): ASID=(%d), new_VSID=(0x%08x),current_VSID=(0x%08x)",
           evt_params->asid, evt_params->vsid, session_obj->vsid );

    /* Update the ASID-VSID mapping as notified. */
    session_obj->vsid = evt_params->vsid;

    break;
  }

  ( void ) __aprv2_cmd_free( vagent_apr_handle, packet );

  return rc;
}

static uint32_t vagent_process_cm_call_event ( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_eventi_cm_call_event_t* evt_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vagent_eventi_cm_call_event_t, packet );
    if ( NULL == evt_params )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    session_obj = &vagent_session_obj_list[evt_params->asid];
    MSG_6( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VAGENT: CM_CALL_EVENT(PROCESS): event=(%d): VSID=(0x%08x): "
           "ASID=(%d): Vagent_state=(%d): call_state(%d) no_of_resources=(%d) ",
           evt_params->event_id, session_obj->vsid, session_obj->asid, 
           session_obj->state, evt_params->call_state, vagent_num_resources );

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

        if ( CM_CONNECTED_CALL_STATUS_CONNECTED == evt_params->call_state )
        {
          session_obj->state = VAGENT_SUBS_STATE_ACTIVE;
        }

        session_obj->is_ring_back_tone_playing = TRUE;/* Replace with is_early_acquisition. */
        vagent_evaluate_resource_grant( session_obj );
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "VAGENT: CM_CALL_EVENT(PROCESS): call_state(%d) Vagent_state(%d) "
               "Early vocoder aquisition", evt_params->call_state, session_obj->state );
      }
      break;

      case CM_CALL_EVENT_CONNECT:
      {
        if ( CM_CONNECTED_CALL_STATUS_CONNECTED == evt_params->call_state )
        {
          /* Update call number here only for MO calls without early ringback tone.
           * 
           * The call number will be updated earlier in the below scenarios, So no need to update again.
           * 1. In case of MO calls with ringback tones the call number will be updated for CM_CALL_EVENT_PROGRESS_INFO_IND.
           * 2. In case of MT calls "CM_CALL_EVENT_ANSWER" event is received and then CM_CALL_EVENT_CONNECT event is received. 
                The call number will be updated for CM_CALL_EVENT_ANSWER event. */
          if( ( FALSE == session_obj->is_ring_back_tone_playing ) && 
              ( VAGENT_SUBS_STATE_INACTIVE == session_obj->state ) )
          {
            ( void ) voicelog_session_update( session_obj->vsid, VOICELOG_INFO_ENUM_CALL_NUMBER );
          }

          /* Clear is_ring_back_tone_playing flag as call is CONNECTED. */
          session_obj->is_ring_back_tone_playing = FALSE;
          session_obj->state = VAGENT_SUBS_STATE_ACTIVE;
          vagent_evaluate_resource_grant( session_obj );
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
                 "VAGENT: CM_CALL_EVENT(PROCESS): "
                 "call_state(%d) Vagent_state(%d) is_ring_back_tone_playing(%d)", 
                 evt_params->call_state, session_obj->state, session_obj->is_ring_back_tone_playing );
        }
      }
      break;

      /* In case of 3rd party IMS CM doesn't broadcast call events hence VAgent
       * state would be INACTIVE. Hence incase of SRVCC VAgent needs to depends
       * on HO_START evt to grant/revoke the resource. */
      case CM_CALL_EVENT_HO_START:
      {
        /*NOTE: This is needed if HO start comes post XVA_REQUEST during SRVCC. */
        if ( ( ( SYS_VOICE_HO_SRVCC_L_2_G == evt_params->ho_type ) ||
                ( SYS_VOICE_HO_SRVCC_L_2_W == evt_params->ho_type ) ||
                ( SYS_VOICE_HO_SRVCC_L_2_C == evt_params->ho_type ) ) &&
               ( VAGENT_VOICE_CLIENT_IMS == session_obj->current_module ) )
        {
          /* Needed for third party IMS SRVCC Latency. */
          vagent_evaluate_resource_grant( session_obj );
        }

        session_obj->ho_type = evt_params->ho_type;
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "VAGENT: CM_CALL_EVENT(PROCESS): alert_media(%d), vagent_state(%d), "
               "is_ring_back_tone_playing(%d)", evt_params->alert_media_type,
               session_obj->state, session_obj->is_ring_back_tone_playing );
      }
      break;

      case CM_CALL_EVENT_HO_COMPLETE:
      {
        if ( CM_CONNECTED_CALL_STATUS_CONNECTED == evt_params->call_state )
        {
          session_obj->state = VAGENT_SUBS_STATE_ACTIVE;
          vagent_evaluate_resource_grant( session_obj );
        }
        else /* ( CM_CONNECTED_CALL_STATUS_NOT_CONNECTED == evt_params->call_state ) */
        {
          if ( ( ( SYS_VOICE_HO_SRVCC_L_2_G == evt_params->ho_type ) ||
                  ( SYS_VOICE_HO_SRVCC_L_2_W == evt_params->ho_type ) ||
                  ( SYS_VOICE_HO_SRVCC_L_2_C == evt_params->ho_type ) ) &&
               ( CM_SRVCC_ALERTING_NW == evt_params->alert_media_type ) )
          {
            /* Enabling to track if early vocoder acqusition is needed. */
            session_obj->is_ring_back_tone_playing = TRUE;
            vagent_evaluate_resource_grant( session_obj );
          }
        }

        session_obj->ho_type = SYS_VOICE_HO_NONE;
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "VAGENT: CM_CALL_EVENT(PROCESS): alert_media(%d), vagent_state(%d), "
               "is_ring_back_tone_playing(%d)", evt_params->alert_media_type,
               session_obj->state, session_obj->is_ring_back_tone_playing );
      }
      break;

      case CM_CALL_EVENT_END:
      {
        /* is_ring_back_tone_playing flag should be cleared when CALL_END event
         * is received in following circumstances:
         *    --when call is CONNECTED and state is ACTIVE or
         *    --when call is NOT_CONNECTED and state is INACTIVE.  
         * In a normal call start and call end scenario,
         * the flag will be cleared as part of handling
         * CM_CALL_EVENT_CONNECT or CM_CALL_EVENT_ANSWER.
         * There are few scenarios as following, during
         * which the flag should not be cleared as part of CALL_END evt.
         *    --ACTIVE CALL is put on HOLD and a second MO call is triggered.
         *    --Now the first call(which is on-hold) is terminated by the far-end user.
         *    --As part of CALL_END evt the flag should not be cleared
         *    --until the second MO call is connected.
         */
        session_obj->ho_type = SYS_VOICE_HO_NONE;

        if ( CM_CONNECTED_CALL_STATUS_NOT_CONNECTED == evt_params->call_state ) 
        {
          if ( VAGENT_SUBS_STATE_INACTIVE != session_obj->state )
          {
            session_obj->state = VAGENT_SUBS_STATE_INACTIVE;
          }
          else 
          {
            session_obj->is_ring_back_tone_playing = FALSE;
          }
        }
        else if ( CM_CONNECTED_CALL_STATUS_CONNECTED == evt_params->call_state ) 
        {
          if ( VAGENT_SUBS_STATE_INACTIVE != session_obj->state )
          {
            session_obj->is_ring_back_tone_playing = FALSE;
          }
          else
          {
            MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "VAGENT: CM_CALL_EVENT(PROCESS):unexpected state as part of CALL_END evt"
                " call_state(%d) vagent_state(%d) is_ring_back_tone_playing(%d)", 
                 evt_params->call_state, session_obj->state, session_obj->is_ring_back_tone_playing );
          }
        }
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "VAGENT: CM_CALL_EVENT(PROCESS):CALL_END"
                " call_state(%d) vagent_state(%d) is_ring_back_tone_playing(%d)", 
                 evt_params->call_state, session_obj->state, session_obj->is_ring_back_tone_playing );
      }
      break;

      default:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VAGENT: CM_CALL_EVENT(PROCESS): Vagent_state(%d)Unhandled cm_event(%d) ",
               session_obj->state, evt_params->event_id );
      }
      break;
    }

    break;
  }

  ( void ) __aprv2_cmd_free( vagent_apr_handle, packet );

  return rc;
}

static uint32_t vagent_process_resource_request_cmd ( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_ivoice_cmd_resource_request_t *cmd_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_cmd_resource_request_t, packet );
    if ( NULL == cmd_params )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    if( cmd_params->client_id >= VAGENT_VOICE_CLIENT_MAX )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    session_obj = &vagent_session_obj_list[cmd_params->asid];

    if ( VAGENT_VOICE_CLIENT_ECALL == cmd_params->client_id )
    {
      /* ECall run concurrently with other module and also ECall resource request/grant
       * works with the design assumption that eCall request for vocoder only after 
       * the G/W call is CONNECTED.
       * Hence resource needs to be granted to ecall along with either of the
       * voice adapter together. */
       session_obj->ecall_resource_request = TRUE;
      ( void ) vagent_resource_grant ( session_obj, VAGENT_VOICE_CLIENT_ECALL );
      break;
    }

    /* IMS is granted the vocoder irrespective of vagent state. */
    if ( session_obj->current_module == cmd_params->client_id )
    {
      MSG_SPRINTF_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
                     "VAGENT: RESOURCE_REQUEST(PROCESS): ASID=(%d): VSID=(0x%08x): state=(%d) "
                     "resource already granted to client[%d]", session_obj->asid, session_obj->vsid,
                     session_obj->state, vagent_client_enum_table[cmd_params->client_id] );
      break;
    }

    /* Cache the resource in pending module. */
    session_obj->pending_module = cmd_params->client_id;
    MSG_SPRINTF_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
                   "VAGENT: RESOURCE_REQUEST(PROCESS): ASID=(%d): VSID=(0x%08x): state=(%d) "
                   "Caching the request for client[%s]", session_obj->asid, session_obj->vsid,
                   session_obj->state, vagent_client_enum_table[cmd_params->client_id] );

    if ( VAGENT_VOICE_CLIENT_IMS == cmd_params->client_id )
    {
      /* Resource granted to IMS irrespective of voice agent state. */
      vagent_evaluate_resource_grant( session_obj );
      break;
    }

    /* Process the request from CS Adapters only if in ACTIVE state or if RBT
     * is in-progress else cache the request in pending_module. */
    if ( ( VAGENT_SUBS_STATE_ACTIVE == session_obj->state ) ||
         ( TRUE == session_obj->is_ring_back_tone_playing ) )
    {
      vagent_evaluate_resource_grant( session_obj );
      break;
    }

    if ( ( ( session_obj->ho_type == SYS_VOICE_HO_SRVCC_L_2_G ) ||
                ( session_obj->ho_type == SYS_VOICE_HO_SRVCC_L_2_W ) ||
                ( session_obj->ho_type == SYS_VOICE_HO_SRVCC_L_2_C ) ) &&
              ( VAGENT_VOICE_CLIENT_IMS == session_obj->current_module) )
    {
      /* Needed for third party IMS SRVCC Latency. */
      vagent_evaluate_resource_grant( session_obj );
      break;
    }

    break;
  }

  ( void ) __aprv2_cmd_free( vagent_apr_handle, packet );

  return rc;
}

static uint32_t vagent_process_resource_release_cmd ( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vagent_session_object_t* session_obj = NULL;
  vagent_ivoice_cmd_resource_release_t *cmd_params = NULL;

  for ( ;; )
  {
    if( packet == NULL )
    {
      VAGENT_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_cmd_resource_release_t, packet );
    if ( NULL == cmd_params )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      break;
    }

    if( cmd_params->client_id >= VAGENT_VOICE_CLIENT_MAX )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    session_obj = &vagent_session_obj_list[cmd_params->asid];

    if ( VAGENT_VOICE_CLIENT_ECALL == cmd_params->client_id )
    {
      session_obj->ecall_resource_request = FALSE;
      break;
    }

    if ( session_obj->pending_module == cmd_params->client_id )
    {
      /* Clearing cached request(if any). */
      session_obj->pending_module = VAGENT_VOICE_CLIENT_INVALID;
      MSG_SPRINTF_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "VAGENT: RESOURCE_RELEASE(PROCESS): ASID=(%d): VSID=(0x%08x): state=(%d) "
                     "clearing pending request from client[%d]", session_obj->asid, session_obj->vsid,
                    session_obj->state, vagent_client_enum_table[cmd_params->client_id] );
      break;
    }

    if (VAGENT_VOICE_CLIENT_INVALID == session_obj->current_module )
    {
      MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "VAGENT: RESOURCE_RELEASE(PROCESS): ASID=(%d): VSID=(0x%08x): state=(%d) "
                     "current client is invalid", session_obj->asid, session_obj->vsid,
                    session_obj->state);
      break;
    }
    else if ( session_obj->current_module !=  cmd_params->client_id )
    {
      MSG_SPRINTF_5( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "VAGENT: RESOURCE_RELEASE(PROCESS): ASID=(%d): VSID=(0x%08x): state=(%d)"
                     "ignoring release event from [%s] as current client[%s]",
                     session_obj->asid, session_obj->vsid, session_obj->state,
                     vagent_client_enum_table[cmd_params->client_id],
                     vagent_client_enum_table[session_obj->current_module] );
      break;
    }

    vagent_num_resources++;
    session_obj->current_module = VAGENT_VOICE_CLIENT_INVALID;
    MSG_SPRINTF_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VAGENT: RESOURCE_RELEASE(PROCESS): ASID=(%d): VSID=(0x%08x): state=(%d)"
                   "release from client[%s]:  vagent_num_resources++(%d)",
                   session_obj->asid, session_obj->vsid, session_obj->state,
                   vagent_client_enum_table[cmd_params->client_id], vagent_num_resources );

    if ( VAGENT_VOICE_CLIENT_IMS == session_obj->pending_module )
    {
      /* Resource granted to IMS irrespective of voice agent state. */
      vagent_evaluate_resource_grant( session_obj );
      break;
    }

    if ( ( TRUE == session_obj->is_ring_back_tone_playing ) ||
         ( VAGENT_SUBS_STATE_ACTIVE == session_obj->state ) )
    {
      /* Voice agent shall remain in active state as audio channel still needs 
       * to kept open for CS domain. */
       vagent_evaluate_resource_grant( session_obj );
       break;
    }

    if ( ( SYS_VOICE_HO_SRVCC_L_2_G == session_obj->ho_type ) ||
         ( SYS_VOICE_HO_SRVCC_L_2_W == session_obj->ho_type ) ||
         ( SYS_VOICE_HO_SRVCC_L_2_C == session_obj->ho_type ) )
    {
      // Needed for third party IMS SRVCC Latency during early media.
      vagent_evaluate_resource_grant( session_obj );
      break;
    }

    break;
  }

  ( void ) __aprv2_cmd_free( vagent_apr_handle, packet );

  return rc;
}

static int32_t vagent_process_client_registration_cmd (
  void *params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;
  vagent_ivoice_cmd_register_t* cmd_params = NULL;

  for ( ;; )
  {
    if ( size != sizeof( vagent_ivoice_cmd_register_t ) )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    cmd_params = (vagent_ivoice_cmd_register_t*)params;
    if ( NULL == cmd_params )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    if( cmd_params->client_id >= VAGENT_VOICE_CLIENT_MAX )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    vagent_client_registration_table[cmd_params->client_id].client_data = cmd_params->client_data;
    vagent_client_registration_table[cmd_params->client_id].client_cb = cmd_params->client_cb;

    break;
  }

  return rc;
}

static int32_t vagent_process_client_deregistration_cmd (
  void *params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;
  vagent_ivoice_cmd_deregister_t* cmd_params = NULL;

  for ( ;; )
  {
    if ( size != sizeof( vagent_ivoice_cmd_deregister_t ) )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    cmd_params = (vagent_ivoice_cmd_deregister_t*)params;
    if ( NULL == cmd_params )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    if( cmd_params->client_id >= VAGENT_VOICE_CLIENT_MAX )
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
      rc = APR_EBADPARAM;
      break;
    }

    vagent_client_registration_table[cmd_params->client_id].client_data = NULL;
    vagent_client_registration_table[cmd_params->client_id].client_cb = NULL;

    break;
  }

  return rc;
}

/****************************************************************************
 * TASK ROUTINES                                                        *
 ****************************************************************************/

static void vagent_process_event_packets (
  void
)
{
  uint32_t rc = APR_EOK;
  uint32_t request_id = 0;
  vagent_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;

  while( apr_list_remove_head( &vagent_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &vagent_free_work_pkt_q, &work_item->link );

    switch( packet->opcode )
    {
      /**
       * Handling routine for nongating work-items should take of release the
       * memory allocated for the CMD/EVENT packets.
       */
      case VAGENT_IVOICE_CMD_RESOURCE_REQUEST:
        ( void ) vagent_process_resource_request_cmd( packet );
        break;

      case VAGENT_IVOICE_CMD_RESOURCE_RELEASE:
        ( void ) vagent_process_resource_release_cmd( packet );
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
        rc = __aprv2_cmd_free( vagent_apr_handle, packet );
        break;
    }

  }

  return;
}

/* This function executes in calling task (server) context */
static int32_t vagent_run (
  void
)
{
  vagent_process_event_packets( );
  return APR_EOK;
}

/* Voice agent's main worker thread function */
static int32_t vagent_worker_thread_fn (
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

static int32_t vagent_init (
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
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vagent_int_lock );
    apr_event_create( &vagent_thread_create_event );
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


static int32_t vagent_postinit ( 
  void
)
{
  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  bool_t ret_val = FALSE;
  cm_client_id_type vagent_client_id = CM_CLIENT_TYPE_NONE;

  rc = __aprv2_cmd_register2(
         &vagent_apr_handle, vagent_my_dns, sizeof( vagent_my_dns ), 0,
         vagent_isr_dispatch_fn, NULL, &vagent_my_addr );
  VAGENT_PANIC_ON_ERROR(rc);

  /* Initialize the session object list. */
  for( index = 0; index < VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V; ++index )
  {
    vagent_session_obj_list[index].header.type = VAGENT_OBJECT_TYPE_ENUM_SESSION;
    vagent_session_obj_list[index].asid = (sys_modem_as_id_e_type)index;
    vagent_session_obj_list[index].vsid = vagent_vsid_table[index];

    vagent_session_obj_list[index].state = VAGENT_SUBS_STATE_INACTIVE;
    vagent_session_obj_list[index].ho_type = SYS_VOICE_HO_NONE;
    vagent_session_obj_list[index].is_ring_back_tone_playing = FALSE;
    vagent_session_obj_list[index].current_module = VAGENT_VOICE_CLIENT_INVALID;
    vagent_session_obj_list[index].pending_module = VAGENT_VOICE_CLIENT_INVALID;
    vagent_session_obj_list[index].ecall_resource_request = FALSE;

    ( void ) voicelog_session_init( vagent_vsid_table[index] );
  }

  /* Initialize the client registration table. */
  for( index = 0; index < VAGENT_VOICE_CLIENT_MAX; ++index )
  {
    vagent_client_registration_table[index].client_id = (vagent_voice_client_type_t)index;
    vagent_client_registration_table[index].client_cb = NULL;
    vagent_client_registration_table[index].client_data = NULL;
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

    /* Regsiter for CM call events in the range [CM_CALL_EVENT_ANSWER -
     * CM_CALL_EVENT_CONNECT]. */
    cm_mm_client_call_reg( vagent_client_id, vagent_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG, CM_CALL_EVENT_ANSWER,
                           CM_CALL_EVENT_CONNECT, NULL );

    /* Register for CM call event CM_CALL_EVENT_PROGRESS_INFO_IND
     * This event is applicable to GSM, WCDMA, TDSCDMA and IMS. */
    cm_mm_client_call_reg( vagent_client_id, vagent_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG, CM_CALL_EVENT_PROGRESS_INFO_IND,
                           CM_CALL_EVENT_PROGRESS_INFO_IND, NULL );

    /* CM registration for CM call events for SRVCC - for handling 3rd party 
     * IMS that does not go through Voice Agent */
    cm_mm_client_call_reg( vagent_client_id, vagent_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG, CM_CALL_EVENT_HO_START,
                           CM_CALL_EVENT_HO_START, NULL );

    /* CM registration for CM call events for SRVCC - for handling 3rd party 
     * IMS that does not go through Voice Agent */
    cm_mm_client_call_reg( vagent_client_id, vagent_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG, CM_CALL_EVENT_HO_COMPLETE,
                           CM_CALL_EVENT_HO_COMPLETE, NULL );
  }
  else
  {
    VAGENT_REPORT_FATAL_ON_ERROR( rc );
  }

  /* TODO: CM regitration for LCH/ACTIVE events per subscription for DSDA. */

  /* Required, if CM has already broadcasted CM phone event before vagent
   * boot-up initialization. */
  for( index = 0; index < VAGENT_MAX_NUM_OF_SUBSCRIPTIONS_V; ++index )
  {
    /* CM will provide the mapping through the CB API registered
     * via cm_client_subs_reg().
     */
    ret_val = cm_ph_cmd_get_subs_pref_info( NULL, NULL, vagent_client_id,
                                            ( sys_modem_as_id_e_type ) index );
    if ( ret_val == FALSE ) 
    {
      VAGENT_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    }
  }

  return rc;
}


static int32_t vagent_predeinit (
  void
)
{
  uint32_t rc = APR_EOK;
  return rc;
}


static int32_t vagent_deinit (
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

    case VAGENT_IVOICE_CMD_REGISTER:
      rc = vagent_process_client_registration_cmd( params, size );
      break;

    case VAGENT_IVOICE_CMD_DEREGISTER:
      rc = vagent_process_client_deregistration_cmd( params, size );
      break;

    case VAGENT_IVOICE_CMD_RESOURCE_REQUEST:
    case VAGENT_IVOICE_CMD_RESOURCE_RELEASE:
      rc = vagent_prepare_and_dispatch_cmd_packet ( NULL, cmd_id, params, size );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vagent_call(): Unsupported cmd ID (0x%08x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;

}

