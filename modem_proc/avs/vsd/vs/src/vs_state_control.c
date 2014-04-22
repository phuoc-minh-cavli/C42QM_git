/**
  @file vs_voc.c
  @brief This file contains the definitions of the internal functions of VS.
*/

/**
   Copyright (C) 2018-2019 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_state_control.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * HEADER FILES                                                     *
 ****************************************************************************/
#include "vs_i.h"

/****************************************************************************
 * GLOBAL VARIABLES                                                         *
 ****************************************************************************/

static char_t *vs_voc_state_enum_table[VS_VOC_STATE_ENUM_MAX] = 
{
  "UNINITIALIZED",

  "RESET_ENTRY  ",
  "RESET        ",
  "IDLE_ENTRY   ",
  "IDLE         ",
  "RUN_ENTRY    ",
  "RUN          ",

  "INVALID      "
};

static char_t *vs_voc_goal_enum_table[VS_VOC_GOAL_ENUM_MAX] = 
{
  "UNINITIALIZED",
  "NONE         ",

  "CREATE       ",
  "DESTROY      ",
  "CONFIGURE    ",
  "ENABLE       ",
  "DISABLE      ",

  "INVALID"
};

static char_t *vs_voc_action_enum_table[VS_VOC_ACTION_ENUM_MAX] = 
{
  "UNINITIALIZED",
  "NONE",
  "COMPLETE",
  "CONTINUE",

  "START_VOICE",
  "STOP_VOICE",
  "STANDBY_VOICE",
  "CREATE_MVM",
  "DESTROY_MVM",
  "SET_NETWORK_ON_MVM",
  "SET_MEDIA_TYPE_ON_MVM",
  "SET_VOICE_TIMING",
  "MEMORY_MAP",
  "MEMORY_UNMAP",
  "SET_VFR_CONFIG",
  "SET_TIMING_REFERENCE",
  "SET_MAX_SAMPLE_RATE",
  "GET_AVTIMER_OFFSET",

  "CREATE_CVS",
  "DESTROY_CVS",
  "ATTACH_CVS",
  "DETACH_CVS",
  "LISTEN_EAMR",
  "CANCEL_LISTEN_EAMR",
  "SET_MEDIA_TYPE_OVER_CVS",
  "SET_CACHED_STREAM_PROPERTIES",
  "RESYNC_CTM",
  "SET_PKTEXG_MODE",
  "SET_MBOX_CONFIG",
  "REGISTER_OOBTTY",
  "DEREGISTER_OOBTTY",
  "ENABLE_HPCM",
  "DISABLE_HPCM",
  "CONFIGURE_VFR",
  "RELEASE_VFR",
  "GET_CVD_VERSION",

  "INVALID"
};

/****************************************************************************
 * VS VOICE SESSION HELPER FUNCTIONS                                        *
 ****************************************************************************/

static int32_t vs_voc_helper_create_new_goal_control (
  vs_voc_goal_enum_t new_goal,
  vs_session_object_t* session_obj,
  vs_object_t** ret_job_obj
)
{
  int32_t rc;
  vs_simple_job_object_t* job_obj;

  if ( ( ret_job_obj == NULL ) || ( session_obj == NULL ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_helper_create_new_goal_control(): NULL params passed "
           "ctrl=(0x%08x) and session_obj=(0x%08x)", ret_job_obj,
           session_obj );
    VS_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
    return APR_EUNEXPECTED;
  }

  if ( session_obj->session_ctrl.goal != VS_VOC_GOAL_ENUM_NONE )
  {
    /* New Goal cannot be created until previous GOAL is completed. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_helper_create_new_goal_control(): Cannot create new Goal "
           "current goal=(0x%08x)", session_obj->session_ctrl.goal );
    VS_PANIC_ON_ERROR( VS_EPENDING );
  }

  session_obj->session_ctrl.goal = new_goal;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

    /* The goal is reset by the state machine on goal completion. */
  session_obj->session_ctrl.pendjob_handle = job_obj->header.handle;
    /* The pendjob_handle will signal completion. The pendjob_handle is
     * reset by the state machine on goal completion.
     */
  *ret_job_obj = ( ( vs_object_t* ) job_obj );
    /* sequencer_job.subjob_obj stores the job_obj to be checked for
     * goal completion and to be freed by the current gating command
     * control.
     */

  return APR_EOK;
}

static int32_t vs_voc_do_complete_goal (
  vs_session_object_t* session_obj
)
{
  int32_t rc =  APR_EOK;
  aprv2_ibasic_rsp_result_t res;

  /* Complete the pending command and stay in the same state. */
  res.opcode = APR_UNDEFINED_ID_V;
  /* Lock not needed as response is not expected */
  res.status = session_obj->session_ctrl.status;
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ), /* This src_port is used for debugging only. */
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ), /* This dst_port is used for debugging only. */
         session_obj->session_ctrl.pendjob_handle,
         APRV2_IBASIC_RSP_RESULT, &res, sizeof( res ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_do_complete_goal(): Sending response failed with "
           "result=0x%08X", rc );
    /* Inject ERR_FATAL as the VS gets into non recoverable state. */
    VS_PANIC_ON_ERROR( rc );
  }

  session_obj->session_ctrl.transition_job_handle = APR_NULL_V;
  session_obj->session_ctrl.pendjob_handle = APR_NULL_V;
  session_obj->session_ctrl.goal = VS_VOC_GOAL_ENUM_NONE;
  session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
  session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_NONE;

  return APR_EOK;
}

static void vs_voc_log_state_info(
  vs_session_object_t* session_obj
)
{
  static vs_voc_state_enum_t prev_state = VS_VOC_STATE_ENUM_UNINITIALIZED;
  static vs_voc_goal_enum_t prev_goal =  VS_VOC_GOAL_ENUM_UNINITIALIZED;
  static vs_voc_action_enum_t prev_action = VS_VOC_ACTION_ENUM_UNINITIALIZED;
  vs_voc_state_enum_t cur_state;
  vs_voc_goal_enum_t cur_goal;
  vs_voc_action_enum_t cur_action;

  if ( session_obj == NULL) return;

  cur_state = session_obj->session_ctrl.state;
  cur_goal = session_obj->session_ctrl.goal;
  cur_action = session_obj->session_ctrl.action;

  /* Show new state information. */
  if ( ( prev_state != cur_state ) ||
       ( prev_goal != cur_goal ) ||
       ( prev_action != cur_action ) )
  {
    MSG_SPRINTF_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
                   "VS:(0x%08X)  STATE: %s  GOAL: %s  ACTION: %s",
                   session_obj->header.handle,
                   vs_voc_state_enum_table[session_obj->session_ctrl.state],
                   vs_voc_goal_enum_table[session_obj->session_ctrl.goal],
                   vs_voc_action_enum_table[session_obj->session_ctrl.action] );
    /* Update information. */
    prev_state = cur_state;
    prev_goal = cur_goal;
    prev_action = cur_action;
  }

  return;
}

/****************************************************************************
 * VS VOCODER STATE MACHINE CONTROL FUNCTIONS                               *
 ****************************************************************************/

static int32_t vs_voc_state_reset_entry (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  /**
   * End the goal the current for below Scenarios:
   * - Audio-PD is DOWN
   * - ADSP SSR triggered.
   * - last action failed
   */
  if ( FALSE == vs_is_cvd_up() )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VS: VOC_STATE_RESET_ENTRY: CVD is not up");
    session_obj->session_ctrl.status = VS_EFAILED;
    rc = vs_voc_do_complete_goal( session_obj );
  }
  else if ( APR_UNDEFINED_ID_V == session_obj->session_ctrl.status )
  {
    /* Stay in this state till response for last action is available. */
    return APR_EOK;
  }
  else if ( APR_EOK != session_obj->session_ctrl.status)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS: VOC_STATE_RESET_ENTRY: last action failed with result=(0x%08X)",
           session_obj->session_ctrl.status );
    rc = vs_voc_do_complete_goal( session_obj );
  }

  session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET;
  return APR_EIMMEDIATE;
}


static int32_t vs_voc_state_reset (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  switch ( session_obj->session_ctrl.goal )
  {
    case VS_VOC_GOAL_ENUM_NONE:
      break;

    case VS_VOC_GOAL_ENUM_CREATE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case VS_VOC_ACTION_ENUM_NONE:
        case VS_VOC_ACTION_ENUM_REQUEST_MAILBOX_MEMORY_CONFIG:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CREATE_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
          if ( (FALSE == vs_adsp_mbox_memory_config_is_received )
                && ( TRUE == vs_platform_is_fusion ) )
          {
            rc  = vs_voc_action_request_mailbox_memory_config( session_obj );
            return rc;
          }                 
          else
          {
              MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                     "VS: VOC_STATE_RESET: mailbox memory config is not requested from ADSP, "
                     "is_fusion=(%d), is_adsp_mbox_memory_config_received=(%d) ",
                     vs_platform_is_fusion, vs_adsp_mbox_memory_config_is_received );
          }
        }        
        /* Fall through */
        case VS_VOC_ACTION_ENUM_CREATE_MVM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CREATE_CVS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_create_mvm( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_CREATE_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_ATTACH_CVS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_create_cvs( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_ATTACH_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_MEMORY_MAP;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_attach_stream( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_MEMORY_MAP:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_GET_AVTIMER_OFFSET;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_map_memory(session_obj );
        }
        return rc;

       case VS_VOC_ACTION_ENUM_GET_AVTIMER_OFFSET:
       {
         session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
         session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_GET_CVD_VERSION;
         session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

         rc  = vs_voc_action_get_avtimer_offset(session_obj );
       }
       return rc;

       case VS_VOC_ACTION_ENUM_GET_CVD_VERSION:
       {
         session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
         session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
         session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

         rc  = vs_voc_action_get_cvd_version(session_obj );
       }
       return rc;

       case VS_VOC_ACTION_ENUM_COMPLETE:
       {
         /* Complete the GOAL as we already have the CVD resource handles. */
         session_obj->session_ctrl.status = APR_EOK;
         session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
         rc = vs_voc_do_complete_goal( session_obj );
       }
       break;

       default:
         break;
      }
    }
    break;

    case VS_VOC_GOAL_ENUM_DESTROY:
    {
      switch ( session_obj->session_ctrl.action)
      {
        case VS_VOC_ACTION_ENUM_CONTINUE:
        case VS_VOC_ACTION_ENUM_MEMORY_UNMAP:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_DETACH_CVS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

           rc = vs_voc_action_unmap_memory( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_DETACH_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_DESTROY_CVS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

           rc = vs_voc_action_detach_stream( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_DESTROY_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_DESTROY_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_destroy_cvs( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_DESTROY_MVM:
        {
          session_obj->cvs_port = APR_NULL_V;
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_destroy_mvm( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          session_obj->mvm_port = APR_NULL_V;
          session_obj->session_ctrl.status = APR_EOK;
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

        default:
          break;
      }
    }
    break;

    default:
      VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
      break;
  }

  return APR_EOK;
}


static int32_t vs_voc_state_idle_entry (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  /**
   * End the goal the current for below Scenarios:
   * - Audio-PD is DOWN
   * - ADSP SSR triggered.
   * - last action failed
   */
   if ( FALSE == vs_is_cvd_up() )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VS: VOC_STATE_IDLE_ENTRY: CVD is not up");
    session_obj->session_ctrl.status = VS_EFAILED;
    rc = vs_voc_do_complete_goal( session_obj );
  }
  else if ( APR_UNDEFINED_ID_V == session_obj->session_ctrl.status )
  {
    /* Stay in this state till response for last action is available. */
    return APR_EOK;
  }
  else if ( APR_EOK != session_obj->session_ctrl.status )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS: VOC_STATE_IDLE_ENTRY: last action failed with result[0x%08X]",
           session_obj->session_ctrl.status );
    rc = vs_voc_do_complete_goal( session_obj );
  }

  session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
  return APR_EIMMEDIATE;
}


static int32_t vs_voc_state_idle (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  switch ( session_obj->session_ctrl.goal )
  {
    case VS_VOC_GOAL_ENUM_NONE:
      break;

    case VS_VOC_GOAL_ENUM_CONFIGURE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case VS_VOC_ACTION_ENUM_NONE:
        case VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_OVER_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_LISTEN_EAMR;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_media_type_on_stream( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_LISTEN_EAMR:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_REGISTER_OOBTTY;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_listen_eamr( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_REGISTER_OOBTTY:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_register_oobtty(session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_network_type_on_mvm( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_MAX_SAMPLE_RATE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_media_type_on_mvm( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_MAX_SAMPLE_RATE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_max_sample_rate( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_PKTEXG_MODE;
          session_obj->session_ctrl.status = APR_EOK;
          rc = VS_EIMMEDIATE;

          if( NULL !=  session_obj->vocoder.set_cached_param_fn )
          {
            ( void ) session_obj->vocoder.set_cached_param_fn( session_obj );
          }
          else
          {
            MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VS: VOC_STATE_IDLE: "
                                                 "cached_param_fn is not initialized");
          }
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_PKTEXG_MODE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_MBOX_CONFIG;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_pktexg_mode( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_MBOX_CONFIG:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_VFR_CONFIG;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
          ( void ) vs_voc_action_initialize_mbox( session_obj );
          rc = vs_voc_action_set_mbox_config( session_obj );
        }
        return rc;        
        
        case VS_VOC_ACTION_ENUM_SET_VFR_CONFIG:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_VFR_ALLOCATE_AND_SYNC;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
          rc = vs_voc_action_set_vfr_config( session_obj );
        }
        return rc;
        
        case VS_VOC_ACTION_ENUM_VFR_ALLOCATE_AND_SYNC:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_VOICE_TIMING;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
          rc = vs_voc_action_configure_vfr( session_obj );
        }
       //Fallthrough 
        
        case VS_VOC_ACTION_ENUM_SET_VOICE_TIMING:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
          
          if ( TRUE == session_obj->vocoder.timing_config.is_timing_ref_available )
          {
            MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,         
                 "vs_voc_state_idle: Timing parameters is calculated from timing references." );
            ( void )vs_voc_action_calculate_timing_offsets( session_obj );
          }
         else
         {
           MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,         
                 "vs_voc_state_idle: Timing parameters is configured." );
         }
          rc = vs_voc_action_set_voice_timing_params( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_SET_TIMING_REFERENCE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_timing_references( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          session_obj->session_ctrl.status = APR_EOK;
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

    case VS_VOC_GOAL_ENUM_ENABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case VS_VOC_ACTION_ENUM_NONE:
        case VS_VOC_ACTION_ENUM_RESYNC_CTM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_START_VOICE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_resync_ctm( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_START_VOICE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_ENABLE_HPCM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_start_voice( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_ENABLE_HPCM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_EOK;
          rc = APR_EIMMEDIATE;

          if( VS_CLIENT_STATE_ENABLED == session_obj->hpcm.client_state )
          {
            session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
            rc = vs_voc_action_hpcm_start( session_obj );
          }
          else
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VS: VOC_STATE_IDLE: client has not enabled HPCM, state=(%d)",
                   session_obj->hpcm.client_state );
          }
        }
        return rc;

        case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          /* Complete GOAL_ENABLE move to RUN STATE state. */
          session_obj->session_ctrl.status = APR_EOK;
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RUN;
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

        default:
          break;
      }
    }
    break;

    case VS_VOC_GOAL_ENUM_DISABLE:
    {
      switch( session_obj->session_ctrl.action )
      {
        case VS_VOC_ACTION_ENUM_CONTINUE:
        case VS_VOC_ACTION_ENUM_VFR_RELEASE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_DISABLE_HPCM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_release_vfr( session_obj );
        }
        //Fallthrough 
        case VS_VOC_ACTION_ENUM_DISABLE_HPCM:
        {
          /* Lock not needed to state as goal isn't none */
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_STOP_VOICE;
          session_obj->session_ctrl.status = APR_EOK;
          rc = APR_EIMMEDIATE;

          if( VS_CLIENT_STATE_ENABLED == session_obj->hpcm.client_state )
          {
            session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
            rc = vs_voc_action_hpcm_stop( session_obj );
          }
          else
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "VS: VOC_STATE_IDLE: HPCM is not enabled by the client, state=(%d)",
                   session_obj->hpcm.client_state );
          }
        }
        return rc;

        case VS_VOC_ACTION_ENUM_STOP_VOICE:
        {
          /* Lock not needed to state as goal isn't none */
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CANCEL_LISTEN_EAMR;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_stop_voice( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_CANCEL_LISTEN_EAMR:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_DEREGISTER_OOBTTY;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_cancel_eamr( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_DEREGISTER_OOBTTY:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_deregister_oobtty( session_obj );
        }
        return rc;

        case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          /* GOAL DISABLE could get trigger in either of the
           * vocoder state RUN or IDLE. */
          session_obj->session_ctrl.status = APR_EOK;
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

        default:
          VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
          break;
      }
    }
    break;

    case VS_VOC_GOAL_ENUM_DESTROY:
    {
      switch ( session_obj->session_ctrl.action )
      {
        /* Transition to Reset as after below action stream will be no
         * more attached to MVM.  */
        case VS_VOC_ACTION_ENUM_NONE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CONTINUE;
          rc = APR_EIMMEDIATE;
        }
        return rc;

        default:
          VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
          break;
      }
    }
    break;

    default:
      VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
      break;
  }

  return APR_EOK;
}

/**
 * This is state is just a place holder for completeness. IDLE
 * to RUN transition doesn't involves this state.
 */
static int32_t vs_voc_state_run_entry (
  vs_session_object_t* session_obj
)
{
  return APR_EIMMEDIATE;
}

static int32_t vs_voc_state_run (
  vs_session_object_t* session_obj
)
{
  int32_t rc;

  switch( session_obj->session_ctrl.goal )
  {
    case VS_VOC_GOAL_ENUM_NONE:
      break;

    case VS_VOC_GOAL_ENUM_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case VS_VOC_ACTION_ENUM_NONE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CONTINUE;
          rc = APR_EIMMEDIATE;
        }
        return rc;

        default:
          VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
          break;
      }
    }
    break;

    default:
      VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
      break;
  }

  return APR_EIMMEDIATE;
}

static uint32_t vs_voc_state_control (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;

  if ( session_obj == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_voc_state_control(): session_obj is NULL " );
    return VS_EBADPARAM;
  }

  do
  {
     switch ( session_obj->session_ctrl.state )
     {
     case VS_VOC_STATE_ENUM_RESET_ENTRY:
       rc = vs_voc_state_reset_entry( session_obj );
       break;

     case VS_VOC_STATE_ENUM_RESET:
       vs_voc_log_state_info( session_obj );
       rc = vs_voc_state_reset( session_obj );
       break;

     case VS_VOC_STATE_ENUM_IDLE_ENTRY:
       rc = vs_voc_state_idle_entry( session_obj );
       break;

     case VS_VOC_STATE_ENUM_IDLE:
       vs_voc_log_state_info( session_obj );
       rc = vs_voc_state_idle( session_obj );
       break;

     case VS_VOC_STATE_ENUM_RUN_ENTRY:
       rc = vs_voc_state_run_entry( session_obj );
       break;

     case VS_VOC_STATE_ENUM_RUN:
       vs_voc_log_state_info( session_obj );
       rc = vs_voc_state_run( session_obj );
       break;

     default:
       VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
     }
  }
  while ( rc == APR_EIMMEDIATE );

  return rc;
}

uint32_t vs_voc_helper_vocoder_setup_ctrl (
  vs_sequencer_job_object_t* seq_obj,
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    switch ( seq_obj->state )
    {
      case VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE:
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_CREATE, session_obj, &seq_obj->subjob_obj );
        if ( rc )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = rc;
        }
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( FALSE == simple_obj->is_completed )
        {
          ( void ) vs_voc_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: VOCODER_SETUP: sequencer state[%d], status[0x%08x]",
               seq_obj->state, simple_obj->status );
        seq_obj->status = simple_obj->status;
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE;

        if ( ( FALSE == vs_is_cvd_up() ) || ( APR_EOK != seq_obj->status ) )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
        }

        ( void ) vs_mem_free_object( ( vs_object_t* ) simple_obj );
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE:
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_CONFIGURE, session_obj, &seq_obj->subjob_obj );
        if ( rc )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = rc;
        }
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;

        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: VOCODER_SETUP: sequencer state[%d], status[0x%08x]",
               seq_obj->state, simple_obj->status );
        seq_obj->status = simple_obj->status;
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE;

        if ( ( FALSE == vs_is_cvd_up() ) || ( APR_EOK != seq_obj->status ) )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
        }

        ( void ) vs_mem_free_object( ( vs_object_t* ) simple_obj );
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE:
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_ENABLE, session_obj, &seq_obj->subjob_obj );
        if ( rc )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = rc;
        }
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;

        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: VOCODER_SETUP: sequencer state[%d], status[0x%08x]",
               seq_obj->state, simple_obj->status );
        seq_obj->status = simple_obj->status;
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;

        ( void ) vs_mem_free_object( ( vs_object_t* ) simple_obj );
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE:
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_DISABLE, session_obj, &seq_obj->subjob_obj );
        if ( rc )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = rc;
        }
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;

        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: VOCODER_SETUP: sequencer state[%d], status[0x%08x]",
               seq_obj->state, simple_obj->status );
        seq_obj->status = simple_obj->status;
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;

        if ( VS_CLIENT_STATE_CLOSED == session_obj->vocoder.client_state )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY;
        }

        ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY:
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_DESTROY, session_obj, &seq_obj->subjob_obj );
        if ( rc )
        {
          seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = rc;
        }
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;

        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: VOCODER_SETUP: sequencer state[%d], status[0x%08x]",
               seq_obj->state, simple_obj->status );
        seq_obj->status = simple_obj->status;
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;

        ( void ) vs_mem_free_object( (vs_object_t*) simple_obj );
      }
      continue;

      case VS_HELPER_SEQUENCER_ENUM_COMPLETE:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: VOCODER_SETUP: status=(0x%08X)", seq_obj->status );
        rc = APR_EOK;
      }
      break;

      default:
        VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
    }

    break;
  }

  return rc;
}

