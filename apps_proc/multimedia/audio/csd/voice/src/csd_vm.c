/*-----------------------------------------------------------------------------
     Copyright (C) 2012, 2016 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/voice/src/csd_vm.c#2 $
  $DateTime: 2019/01/03 07:23:13 $
  $Author: pwbldsvc $
  $Change: 17964421 $
  $Revision: #2 $
	=========================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.
       when          who       what, where, why
      --------     -----     ---------------------------------------------------
       11/15/16      rk       Abstracting OSAL layer
       03/03/15      rp       Added modes to CSD HDon API to control the beAMR and 
                              Wide Voice2 enable and disable independently. 
       09/01/14      rp       enhanced the csd init debug buffer 
       04/16/14      jk       added support for Voice Session Manager
       02/06/14      vk       modified code for proper csd/acdb/apr error mapping
       05/17/13      aim      Added support for unified CSD msg logging.
       01/23/12      jk       added support for CSD_VM_IOCTL_PAUSE_VOICE API      
       12/06/12      rp       updated csd init debug info for 9x25
       07/18/12      jy       added ioctl for mem mapping       
       04/19/12      rp       Added support for voice standby mode
       03/21/12      jk       added support for VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL 
                              during passive mvm sessions         
       03/07/12      oa       added fix for memory leak during VM close
       02/22/12      jp/rp    added debug messages
       01/19/12      jk       added fix for setting csd_vm_initialized to false during deinit
       12/06/11      jk       added handling for hold passive vs handles for rtc 

=============================================================================*/

#include "csd_vm_private.h"
#include "csd_debug_info.h"
#include "csd_error_map.h"

static uint32_t csd_vm_apr_handle;
static uint8_t  csd_vm_cvs_dest_dns[] = "qcom.adsp.cvs";
static uint8_t  csd_vm_cvp_dest_dns[] = "qcom.adsp.cvp";
static uint8_t  csd_vm_mvm_dest_dns[] = "qcom.adsp.mvm";
static char_t   csd_vm_dns[] = "qcom.apps.csd_vm";
static uint16_t csd_vm_src_addr;
static uint16_t csd_vm_cvs_dest_addr;
static uint16_t csd_vm_cvp_dest_addr;
static uint16_t csd_vm_mvm_dest_addr;
static csd_lock_t csd_vm_task_lock; /* Lock management */


static apr_list_t csd_vm_free_cmd_list;
static apr_list_t csd_vm_active_cmd_list;
static apr_list_t csd_vm_track_dispatched_cmd_list;
static csd_vm_cmd_list_item_t csd_vm_cmd_pool[ CSD_VM_NUM_COMMANDS ];

static apr_list_t csd_vm_free_sess_list;
static apr_list_t csd_vm_active_sess_list;
static csd_vm_session_item_t csd_vm_sess_pool[ CSD_VM_NUM_SESSIONS ];

static apr_objmgr_object_t csd_vm_object_table[ CSD_VM_MAX_OBJECTS_V ];
static apr_objmgr_t csd_vm_apr_objmgr;

static struct csd_vtable csd_vm_vtable;

static uint32_t token_counter = 0; /* Unique Token counter to keep track of attach and detach cmds sent */
static uint32_t h_csd_voc_common = NULL;

/*
 ** Start Internal functions
*/

static void csd_vm_task_lock_fn ( void ) 
{
  ( void ) csd_lock_enter( csd_vm_task_lock );
}

static void csd_vm_task_unlock_fn ( void ) 
{
  ( void ) csd_lock_leave( csd_vm_task_lock );
}

uint32_t csd_vm_store_vp3_data(csd_vm_session_item_t* psess)
{
  uint32_t rc = CSD_EOK;
  uint32_t write = 0;
  uint32_t total = 0;
  csd_vsm_assoc_handle_list_t vc_list;
  csd_vsm_handle_t handle;
  uint32_t buf; /*allow space for only one vc handles*/

  /*initalize list*/
  buf = 0;
  vc_list.buffer_size = sizeof(buf);
  vc_list.ret_total_size = &total;
  vc_list.ret_write_size = &write;
  vc_list.buffer = &buf;
  vc_list.handle = psess->h_csd;
  handle.assoc_mode = TRUE;
  handle.handle_type = CSD_VSM_VC_HANDLE;

  /*get all vc handles associated with this vm */
  rc = csd_vsm_get_handles(handle, CSD_VSM_SESSION_STATE_RUNNING, &vc_list);
  if ( rc == CSD_EOK )
  {
    csd_vc_ioctl_store_vp3_data_t cmd = {0};
    /* Query for VP3 data  */
    rc = csd_vc_ioctl( *vc_list.buffer, CSD_VC_IOCTL_STORE_VP3_DATA,
                               &cmd, sizeof(csd_vc_ioctl_store_vp3_data_t) );
  }
  return rc;
}

uint32_t csd_vm_detached_handles(csd_vm_session_item_t* psess, csd_vsm_handle_enum_t handle_type )
{
  uint32_t rc = CSD_EOK;
  uint32_t write = 0;
  uint32_t total = 0;
  csd_vsm_assoc_handle_list_t h_list;
  csd_vsm_handle_t handle;
  uint32_t buf; /*allow space for only 1 handles*/

  /*initalize list*/
  buf = 0;
  h_list.buffer_size = sizeof(buf);
  h_list.ret_total_size = &total;
  h_list.ret_write_size = &write;
  h_list.buffer = &buf;
  h_list.handle = psess->h_csd;
  handle.assoc_mode = TRUE;
  handle.handle_type = handle_type;

  /*get all vc handles associated with this vm*/
  rc = csd_vsm_get_handles(handle, CSD_VSM_SESSION_STATE_IDLE, &h_list);
  if ( rc == CSD_EOK )
  {
    /* dettach handles */
    if ( handle_type == CSD_VSM_VS_HANDLE )
    {
      csd_vm_ioctl_detach_stream_t detach_cmd;
      detach_cmd.cmd_token = 0;
      detach_cmd.stream_handle = *h_list.buffer;
      rc = csd_vm_ioctl(psess->h_csd, CSD_VM_IOCTL_DETACH_STREAM, &detach_cmd, sizeof(csd_vm_ioctl_detach_stream_t) );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vm_detached_handles: CSD_VM_IOCTL_DETACH_STREAM sent, rc[0x%x]", rc);

      if ( rc == CSD_EOK )
      {
        csd_event_wait ( psess->csd_vm_event );
      }

    }
    else if ( handle_type == CSD_VSM_VC_HANDLE )
    {
      csd_vm_ioctl_detach_context_t detach_cmd;
      detach_cmd.cmd_token = 0;
      detach_cmd.context_handle = *h_list.buffer;
      rc = csd_vm_ioctl(psess->h_csd, CSD_VM_IOCTL_DETACH_CONTEXT, &detach_cmd, sizeof(csd_vm_ioctl_detach_context_t) );

      CSD_MSG( CSD_PRIO_HIGH, "csd_vm_detached_handles: CSD_VM_IOCTL_DETACH_CONTEXT sent, rc[0x%x]", rc);
      if ( rc == CSD_EOK )
      {
        csd_event_wait ( psess->csd_vm_event );
      }
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_close_attached_handles: invalid handle type to detach[%d]", handle_type);
      rc = CSD_EBADPARAM;
    }
  }

  return rc;
}

uint32_t csd_vm_update_state (csd_vm_session_item_t  *psess, uint32_t cmd)
{
  uint32_t rc = CSD_EOK;
  switch( psess->sess_state.state )
  {
    case CSD_VSM_HANDLE_STATE_UNINITIALIZED:
      /* if session is opened transition*/
      if( cmd == VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          rc = csd_vsm_update_state( psess->h_csd, psess->vm_name, psess->sess_state.state );
      }
      /* if passive assume vs is attached*/
      if ( cmd == VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          psess->sess_state.vs_attached = TRUE;
          rc = csd_vsm_update_state( psess->h_csd, psess->vm_name, psess->sess_state.state );     
      }
      break;
    case CSD_VSM_HANDLE_STATE_OPEN:
       /* if session is opened transition*/
      if( cmd == APRV2_IBASIC_CMD_DESTROY_SESSION )
      {
         psess->sess_state.state = CSD_VSM_HANDLE_STATE_UNINITIALIZED;
         rc = csd_vsm_update_state( psess->h_csd, psess->vm_name, psess->sess_state.state );
      }
      else
      {
        if( psess->sess_state.vs_attached && psess->sess_state.vc_attached && psess->sess_state.running )
        {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_READY;
         rc =  csd_vsm_update_state( psess->h_csd, psess->vm_name, psess->sess_state.state );
        }
      }
      break;
    case CSD_VSM_HANDLE_STATE_READY:
       /* if session is opened transition*/
      if( cmd == APRV2_IBASIC_CMD_DESTROY_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_UNINITIALIZED;
          rc = csd_vsm_update_state( psess->h_csd, psess->vm_name, psess->sess_state.state );
      }
      else
      {
        if( !psess->sess_state.vs_attached || !psess->sess_state.vc_attached ||
            !psess->sess_state.running )
        {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          rc = csd_vsm_update_state( psess->h_csd, psess->vm_name, psess->sess_state.state );
        }
      }
      break;
    default:
       CSD_MSG( CSD_PRIO_ERROR, "csd_vs_update_state: Unknown state[%d]", psess->sess_state.state );
       rc = CSD_EBADSTATE;
  }
  return rc;
}

bool_t csd_vm_session_exsists(const char* session_name)
{
  bool_t rc = FALSE;
  csd_vm_session_item_t *link;
  csd_vm_session_item_t *pivot;
   
  /* check if the vs is belonged to this vm */
  if ( !( apr_list_peak_head ( &csd_vm_active_sess_list, ( apr_list_node_t ** )&link) ) )
  {    
    do 
    {
      if ( strncmp( session_name, link->vm_name, link->vm_name_size )== 0 )
      {
        rc = TRUE;
        break;
      }
      pivot = link;
    } while ( ! ( apr_list_get_next ( &csd_vm_active_sess_list,
                                      ( apr_list_node_t* )pivot, ( apr_list_node_t** )&link ) ));
  }
  return rc;
}

uint32_t csd_vm_command_fn( uint32_t param) 
{
  csd_vm_cmd_list_item_t* pcmd = NULL;
  csd_vm_event_command_rsp_t event_rsp;
  int32_t rc = CSD_EOK;

  while ( ! ( apr_list_remove_head( &csd_vm_active_cmd_list, ( apr_list_node_t** )&pcmd ) ) )
  {
    if ( pcmd != NULL )
    {
      switch ( pcmd->cmd_type ) 
      {
        case CSD_VM_CMD_SEND_RSP_CALLBACK:
        {
          if ( pcmd->psess->csd_vm_cb != NULL )
          {
            event_rsp.status_id = pcmd->cmd.csd_vmw_rsp_cmd.status_id;

            ( void )pcmd->psess->csd_vm_cb( pcmd->client_token, 
                                    CSD_VM_EVENTID_COMMAND_RSP,
                                    &event_rsp, 
                                    sizeof ( csd_vm_event_command_rsp_t ), 
                                    pcmd->psess->csd_vm_cb_data );

            CSD_MSG( CSD_PRIO_LOW, "csd_vm_command_fn: CSD_VM_CMD_SEND_RSP_CALLBACK , token[0x%x], result[0x%x]",
                     pcmd->client_token, event_rsp.status_id );
          }
          else
          {
            CSD_MSG( CSD_PRIO_LOW, "csd_vm_command_fn: CSD_VM_CMD_SEND_RSP_CALLBACK, no callback has been registered!" );
          }

          break;
        }
        case CSD_VM_CMD_UPDATE_STATE:
          csd_vm_update_state(pcmd->psess, pcmd->cmd.update_state_cmd);
          break;

        default:
          rc = CSD_EBADPARAM;
        break;
      }
    }
    ( void ) apr_list_add_tail( &csd_vm_free_cmd_list, &pcmd->link );
  }
  return rc;
}

/* not used for csd_vm */
uint32_t csd_vm_state_fn( uint32_t param)
{
  return CSD_EFAILED;
}

static void csd_vm_get_cmd_node ( uint32_t cmd_token, csd_vm_cmd_list_item_t **pcmd_node )
{
  csd_vm_cmd_list_item_t* pcmd_pivot = NULL;
  csd_vm_cmd_list_item_t* pcmd = NULL;

  *pcmd_node = NULL;

  if ( !( apr_list_peak_head ( &csd_vm_track_dispatched_cmd_list, ( apr_list_node_t ** )&pcmd ) ) )
  {
    pcmd_pivot = pcmd;

    do
    {
      if ( pcmd->cmd_token == cmd_token )
      {
        *pcmd_node = pcmd; /* if the token is matching then populate the pcmd_node */
        return;
      }
      else
      {
        pcmd_pivot = pcmd; /* update the pivot pointer */
      }

    } while ( ! ( apr_list_get_next ( &csd_vm_track_dispatched_cmd_list,
              ( apr_list_node_t* )pcmd_pivot, ( apr_list_node_t** )&pcmd ) ) );
  }
}

static int32_t csd_vm_apr_callback ( 
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  aprv2_ibasic_rsp_result_t  *rsp;
  csd_vm_session_item_t *psess = NULL;
  csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_CMD;
  csd_vm_cmd_list_item_t* pcmd = NULL;
  csd_vm_cmd_list_item_t* pcmd2 = NULL;
  csd_vm_cmd_list_item_t* ptracked_cmd = NULL; /* pointer to tracked command item */
  apr_objmgr_object_t* apr_obj = NULL;


  if ( ( packet != NULL ) && 
       ( packet->opcode == APRV2_IBASIC_RSP_RESULT ) )
  {
    ( void ) apr_objmgr_find_object ( &csd_vm_apr_objmgr, packet->dst_port, &apr_obj );

    if ( apr_obj != NULL ) 
    {
      psess = apr_obj->any.ptr;
    }

    rsp = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet);

    if ( ( rsp == NULL ) || ( psess == NULL ) )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_vm_apr_callback: Unable to find session or invalid packet" );
    }
    else
    {
      /* Check to see what opcode this response is for as there are different
         operations to perform on certain opcode responses */
      switch ( rsp->opcode )
      {
        case VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION:
        case VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION:
        {
          if( rsp->status == APR_EOK )
          {
            psess->h_mvm = packet->src_port;            
          }
          ( void ) csd_event_signal ( psess->csd_vm_event );
        break;
        }
        
        case APRV2_IBASIC_CMD_DESTROY_SESSION:
        {
          psess->h_mvm = APR_NULL_V;
          ( void ) csd_event_signal ( psess->csd_vm_event );
        break;
        }
        
        case VSS_IMVM_CMD_ATTACH_VOCPROC:
        case VSS_IMVM_CMD_DETACH_VOCPROC:
        case VSS_IMVM_CMD_ATTACH_STREAM:
        case VSS_IMVM_CMD_DETACH_STREAM:
        {
          /* retreive the client's command request based on unique token */
          ( void )csd_vm_get_cmd_node ( packet->token, &ptracked_cmd );

          /* return the tracked cmd item back to free queue */
          if ( NULL != ptracked_cmd )
          { /* return the list item back into the free cmd list */
            ( void ) apr_list_delete ( &csd_vm_track_dispatched_cmd_list, ( apr_list_node_t* )&ptracked_cmd->link );
            ( void ) apr_list_add_tail ( &csd_vm_free_cmd_list, ( apr_list_node_t* )&ptracked_cmd->link );
          }
          if ( APR_EOK == rsp->status && NULL != ptracked_cmd )
          {
            if ( rsp->opcode == VSS_IMVM_CMD_ATTACH_VOCPROC )
            {
              psess->sess_state.vc_attached = TRUE; 
            }
            else if ( rsp->opcode == VSS_IMVM_CMD_DETACH_VOCPROC )                          
            {
              psess->sess_state.vc_attached = FALSE;
            }
            else if ( rsp->opcode == VSS_IMVM_CMD_ATTACH_STREAM )
            {
              psess->sess_state.vs_attached = TRUE;
            }
            else if ( rsp->opcode == VSS_IMVM_CMD_DETACH_STREAM )             
            {
              psess->sess_state.vs_attached = FALSE;   
            }
          }/* if ( APR_EOK == rsp->status && NULL != ptracked_cmd ) */

          /*send state update*/
          ( void ) apr_list_remove_head( &csd_vm_free_cmd_list, (apr_list_node_t**)&pcmd2 );
          if( pcmd2 )
          {
            pcmd2->cmd_type = CSD_VM_CMD_UPDATE_STATE;
            pcmd2->cmd.update_state_cmd = rsp->opcode;
            pcmd2->psess = psess;
            ( void ) apr_list_add_tail( &csd_vm_active_cmd_list, &pcmd2->link );
            /* signal the csd common thread to process the cmd */
            sigs |= CSD_VOC_COMMON_SIG_CMD;
            ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
          }

          /* if called internal signal else callback client*/
          if ( psess->bclosing_sequence )
          {
            ( void ) csd_event_signal ( psess->csd_vm_event );
          }
          else
          {
          
            /* obtain a free command item */
            ( void ) apr_list_remove_head( &csd_vm_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );

            if ( NULL != pcmd && NULL != ptracked_cmd ) 
            {
              pcmd->cmd_type = CSD_VM_CMD_SEND_RSP_CALLBACK;
              pcmd->psess = psess;
              pcmd->cmd.csd_vmw_rsp_cmd.status_id= rsp->status;
              pcmd->client_token = ptracked_cmd->client_token;

              ( void ) apr_list_add_tail( &csd_vm_active_cmd_list, &pcmd->link );
              /* signal the csd common thread to process the cmd */
              ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
            }
          }
        break;
        } /* ATTACH / DETACH for Stream and Context  */
        
        case VSS_ICOMMON_CMD_SET_NETWORK:
        case VSS_IWIDEVOICE_CMD_SET_WIDEVOICE:
        case VSS_IMVM_CMD_START_VOICE:
        case VSS_IMVM_CMD_STOP_VOICE:
        case VSS_IMVM_CMD_STANDBY_VOICE:
        case VSS_IMVM_CMD_PAUSE_VOICE:
        {          
           if ( APR_EOK == rsp->status )
           {

              if ( rsp->opcode == VSS_IMVM_CMD_START_VOICE )
              {
                psess->sess_state.running = TRUE;
                
                /*send state update*/
                ( void ) apr_list_remove_head( &csd_vm_free_cmd_list, (apr_list_node_t**)&pcmd2 );
                if( pcmd2 )
                {
                  pcmd2->cmd_type = CSD_VM_CMD_UPDATE_STATE;
                  pcmd2->cmd.update_state_cmd = rsp->opcode;
                  pcmd2->psess = psess;
                  ( void ) apr_list_add_tail( &csd_vm_active_cmd_list, &pcmd2->link );
                  /* signal the csd common thread to process the cmd */
                  sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
                }
              }

              else if ( ( rsp->opcode == VSS_IMVM_CMD_STOP_VOICE )
                  || ( rsp->opcode == VSS_IMVM_CMD_STANDBY_VOICE ) 
                  || ( rsp->opcode == VSS_IMVM_CMD_PAUSE_VOICE ) )
              {

                psess->sess_state.running = FALSE;
                
                /*send update*/
                ( void ) apr_list_remove_head( &csd_vm_free_cmd_list, (apr_list_node_t**)&pcmd2 );
                if( pcmd2 )
                {
                  pcmd2->cmd_type = CSD_VM_CMD_UPDATE_STATE;
                  pcmd2->cmd.update_state_cmd = rsp->opcode;
                  pcmd2->psess = psess;
                  ( void ) apr_list_add_tail( &csd_vm_active_cmd_list, &pcmd2->link );
                  /* signal the csd common thread to process the cmd */
                  sigs |= CSD_VOC_COMMON_SIG_CMD;
                  ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
                }
              } /*  if ( APR_EOK == rsp->status ) */
           }

           /* if called internal signal else callback client*/
          if ( psess->bclosing_sequence )
          {
            ( void ) csd_event_signal ( psess->csd_vm_event );
          }
          else
          {
            /* for these commands only send a callback */
            ( void ) apr_list_remove_head( &csd_vm_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );

            if ( pcmd != NULL) 
            {
              pcmd->cmd_type = CSD_VM_CMD_SEND_RSP_CALLBACK;
              pcmd->psess = psess;
              pcmd->cmd.csd_vmw_rsp_cmd.status_id= rsp->status;
              pcmd->client_token = packet->token;

              ( void ) apr_list_add_tail( &csd_vm_active_cmd_list, &pcmd->link );
               /* signal the csd common thread to process the cmd */
              ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
            }      
          }
        break;
        } /* cases VSS_IMVM_CMD_STOP_VOICE */

        case VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL:
          ( void ) csd_event_signal ( psess->csd_vm_event );
        break;

        case VSS_IHDVOICE_CMD_ENABLE:
        case VSS_IHDVOICE_CMD_DISABLE:
        case VSS_IMVM_CMD_SET_CAL_NETWORK:
        case VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE:
        case VSS_ICOMMON_CMD_SET_VOICE_TIMING:
        case VSS_ITTY_CMD_SET_TTY_MODE:
        {
          /* for these commands only send a callback */
          ( void ) apr_list_remove_head( &csd_vm_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );

          if ( pcmd != NULL) 
          {
            pcmd->cmd_type = CSD_VM_CMD_SEND_RSP_CALLBACK;
            pcmd->psess = psess;
            pcmd->cmd.csd_vmw_rsp_cmd.status_id= rsp->status;
            pcmd->client_token = packet->token;

            ( void ) apr_list_add_tail( &csd_vm_active_cmd_list, &pcmd->link );
            /* signal the csd common thread to process the cmd */
            ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
          }
          break;
        }
		
        case VSS_IHDVOICE_CMD_BEAMR_DISABLE:
        case VSS_IHDVOICE_CMD_WV2_DISABLE:
        {
     	    CSD_MSG( CSD_PRIO_HIGH, "csd_vm_apr_callback: HDVOICE BEAMR_DISABLE/WV2_DIABLE rsp[%d]", rsp->status );
          csd_event_signal( psess->csd_vm_event );
          break;
        }
		
        default:
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_vm_apr_callback: Response received for unknown opcode");
          break;
        }
      } /* switch ( rsp->opcode ) */
    } /* else case if ( ( rsp == NULL ) || ( psess == NULL ) ) */
  }/* if ( ( packet != NULL ) && ( packet->opcode == APRV2_IBASIC_RSP_RESULT ) ) */
  else if ( ( packet != NULL ) && ( packet->opcode != APRV2_IBASIC_EVT_ACCEPTED ) )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_vm_apr_callback: Unable to recognize opcode, opcode[0x%x]", packet->opcode );
  }

  if ( NULL != packet )
  {
    __aprv2_cmd_free( csd_vm_apr_handle, packet );
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_vm_apr_callback: Null packet");
    return APR_EFAILED;
  }

  /* Note that rc needs to be APR_EOK in order to prevent APR from freeing the packet a second time */
  return APR_EOK;
}

/* uint32_t () {*/ /* DONE!!*/
CSD_INTERNAL int32_t csd_vm_init ( struct csd_vtable** vtable )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint8_t i = 0;
  uint8_t checkpoint = 0;

  {
    rc = csd_voc_common_get_session( csd_vm_command_fn,
                                     &h_csd_voc_common );
  }

  if( ( CSD_EOK == rc ) && vtable ) 
  {
    checkpoint = 1;
    /* CSD vm get session success */
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_VM_INIT_STAGE,
                       //CSD_INIT_VM_SESSION_GET,0,0); 
    csd_vm_vtable.open_fn = ( csd_open_fn ) csd_vm_open;
    csd_vm_vtable.close_fn = ( csd_close_fn )csd_vm_close;
    csd_vm_vtable.ioctl_fn = ( csd_ioctl_fn )csd_vm_ioctl;
    csd_vm_vtable.read_fn = ( csd_read_fn )csd_vm_read;
    csd_vm_vtable.write_fn = ( csd_write_fn ) csd_vm_write;
    *vtable = &csd_vm_vtable;

    apr_rc = __aprv2_cmd_local_dns_lookup( (char_t*)csd_vm_cvs_dest_dns,
                                         sizeof( csd_vm_cvs_dest_dns),
                                         &csd_vm_cvs_dest_addr);
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                         //CSD_INIT_VM_DNS_CVS_LOOK_UP,0,0); 


    if ( APR_EOK == apr_rc ) 
    {
      /* CSD vm cvs dns lookup session success */
      apr_rc = __aprv2_cmd_local_dns_lookup( (char_t*)csd_vm_cvp_dest_dns,
                                         sizeof( csd_vm_cvp_dest_dns),
                                         &csd_vm_cvp_dest_addr);
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_DNS_CVP_LOOK_UP,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      apr_rc = __aprv2_cmd_local_dns_lookup( (char_t*)csd_vm_mvm_dest_dns,
                                         sizeof( csd_vm_mvm_dest_dns),
                                         &csd_vm_mvm_dest_addr);
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_DNS_MVM_LOOK_UP,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      apr_rc = __aprv2_cmd_register2( &csd_vm_apr_handle,
                                  csd_vm_dns,
                                  sizeof ( csd_vm_dns ),
                                  0,
                                  csd_vm_apr_callback,
                                  NULL,
                                  &csd_vm_src_addr );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_APR_REGISTER,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 2;
      apr_rc = csd_lock_create( &csd_vm_task_lock );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_APR_LOCK_CREATE,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 3;
      apr_rc = apr_list_init( &csd_vm_free_cmd_list, csd_vm_task_lock_fn, csd_vm_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_FREE_CMD_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      checkpoint = 4;
      apr_rc = apr_list_init( &csd_vm_active_cmd_list, csd_vm_task_lock_fn, csd_vm_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_ACTIVE_CMD_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      checkpoint = 5;
      apr_rc = apr_list_init( &csd_vm_track_dispatched_cmd_list, csd_vm_task_lock_fn, csd_vm_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_DISPATCH_CMD_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc )
    {
      checkpoint = 6;
      for ( i = 0; i < CSD_VM_NUM_COMMANDS; ++i ) 
      {
        if ( APR_EOK != ( apr_rc = apr_list_add_tail( &csd_vm_free_cmd_list,
                                                ( apr_list_node_t* ) &csd_vm_cmd_pool[i] ) ) )
        {
          break;
        }
      }
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_FREE_CMD_LIST_ADD,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      apr_rc = apr_list_init( &csd_vm_free_sess_list, csd_vm_task_lock_fn, csd_vm_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                          // CSD_INIT_VM_FREE_SESS_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 7;
      apr_rc = apr_list_init( &csd_vm_active_sess_list, csd_vm_task_lock_fn, csd_vm_task_unlock_fn );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                           //CSD_INIT_VM_ACTIVE_SESS_LIST_INIT,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      checkpoint = 8;
      for ( i = 0; i < CSD_VM_NUM_SESSIONS; ++i ) 
      {
        if ( APR_EOK != ( apr_rc = apr_list_add_tail( &csd_vm_free_sess_list,
                              ( ( apr_list_node_t* ) &(csd_vm_sess_pool[i].link) ) ) ) )
        {
          break;
        }
      }
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                //           CSD_INIT_VM_FREE_SESS_LIST_ADD,0,0); 
    }

    if ( APR_EOK == apr_rc ) 
    {
      /* Initialize the object manager. */
      apr_objmgr_setup_params_t params;
      params.table = csd_vm_object_table;
      params.total_bits = CSD_VM_HANDLE_TOTAL_BITS_V;
      params.index_bits = CSD_VM_HANDLE_INDEX_BITS_V;
      params.lock_fn = csd_vm_task_lock_fn;
      params.unlock_fn = csd_vm_task_unlock_fn;
      apr_rc = apr_objmgr_construct( &csd_vm_apr_objmgr, &params );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VM_INIT_STAGE,
                  //         CSD_INIT_VM_OBJECT_MGR_INIT,0,0); 
    }

    if ( APR_EOK != apr_rc ) 
    {
      /* free resources here*/

      CSD_MSG( CSD_PRIO_ERROR, "csd_vm_init: Error with initialization, checkpoint[%d]", checkpoint );

      switch ( checkpoint ) 
      {
        case 8:
          ( void ) apr_list_destroy ( &csd_vm_active_sess_list );
          /*csd vm active session list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                    //         CSD_INIT_VM_ACTIVE_SESS_LIST_INIT,0,0); 
        case 7:
          ( void ) apr_list_destroy ( &csd_vm_free_sess_list );
          /*csd vm free session list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                      //       CSD_INIT_VM_FREE_SESS_LIST_INIT,0,0); 
        case 6:
          ( void ) apr_list_destroy ( &csd_vm_track_dispatched_cmd_list );
          /*csd vm dispatch command list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                        //     CSD_INIT_VM_DISPATCH_CMD_LIST_INIT,0,0); 
        case 5:
          ( void ) apr_list_destroy ( &csd_vm_active_cmd_list );        
          /*csd vm active command list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                          //   CSD_INIT_VM_ACTIVE_CMD_LIST_INIT,0,0); 
        case 4:
          ( void ) apr_list_destroy ( &csd_vm_free_cmd_list );
          /*csd vm free command list destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                            // CSD_INIT_VM_FREE_CMD_LIST_INIT,0,0); 
        case 3:
          ( void ) csd_lock_destroy ( csd_vm_task_lock );
          /*csd vm apr lock destroy done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                             //CSD_INIT_VM_APR_LOCK_CREATE,0,0); 
        case 2:
          ( void ) __aprv2_cmd_deregister( csd_vm_apr_handle);
          /*csd vm apr de-register done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                             //CSD_INIT_VM_APR_REGISTER,0,0); 
        case 1:
          ( void ) csd_voc_common_free_session ( &h_csd_voc_common );
          /*csd vm release session done*/
          //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
                             //CSD_INIT_VM_SESSION_GET,0,0); 
        break;
        default:
        break;
      }/* switch */
    }
    rc = csd_map_apr_error_to_csd( apr_rc );
  }

  return rc;
}

CSD_INTERNAL int32_t csd_vm_deinit ( void ) /* need to add counter */
{
  ( void ) csd_voc_common_free_session ( &h_csd_voc_common );
  /*csd vm release session done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_VM_INIT_STAGE,
                     //CSD_INIT_VM_SESSION_GET,0,0); 

  ( void ) __aprv2_cmd_deregister( csd_vm_apr_handle);
  /*csd vm apr de-register done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
//                     CSD_INIT_VM_APR_REGISTER,0,0); 

  ( void ) apr_list_destroy( &csd_vm_free_cmd_list);
  /*csd vm free command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
  //                   CSD_INIT_VM_FREE_CMD_LIST_INIT,0,0);

  ( void ) apr_list_destroy( &csd_vm_active_cmd_list);
  /*csd vm active command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
    //                 CSD_INIT_VM_ACTIVE_CMD_LIST_INIT,0,0);

  ( void ) apr_list_destroy ( &csd_vm_track_dispatched_cmd_list );
  /*csd vm dispatch command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
      //               CSD_INIT_VM_DISPATCH_CMD_LIST_INIT,0,0);

  ( void ) apr_list_destroy ( &csd_vm_active_sess_list );
  /*csd vm active session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
        //             CSD_INIT_VM_ACTIVE_SESS_LIST_INIT,0,0);

  ( void ) apr_list_destroy ( &csd_vm_free_sess_list );
  /*csd vm free session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
          //           CSD_INIT_VM_FREE_SESS_LIST_INIT,0,0);

  ( void ) apr_objmgr_destruct( &csd_vm_apr_objmgr );
  /*csd vm object manager destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
            //         CSD_INIT_VM_OBJECT_MGR_INIT,0,0);

  ( void ) csd_lock_destroy ( csd_vm_task_lock );
  /*csd vm apr lock destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VM_INIT_STAGE,
              //       CSD_INIT_VM_APR_LOCK_CREATE,0,0);

  return CSD_EOK;
}

/* this call needs to be syncronous */
CSD_INTERNAL int32_t csd_vm_open (
  uint32_t open_id,
  void* params,
  uint32_t size )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vm_session_item_t* psess = NULL;
  apr_objmgr_object_t* objmgr_obj = NULL;
  csd_vm_open_t* csd_vm_cmd = ( csd_vm_open_t* )params;
  vss_imvm_cmd_set_policy_dual_control_t dual_control;

   /*validate session is unique*/
  if( csd_vm_session_exsists(csd_vm_cmd->u.full_control.name) )
  {
    rc = CSD_EALREADY;
     CSD_MSG( CSD_PRIO_ERROR, "csd_vm_open: Failed!! Session with same name already exsists");
  }
  else
  {
    /* Get a session */
    apr_rc = apr_list_remove_head( &csd_vm_free_sess_list, ( ( apr_list_node_t** )&psess) );

    if ( ( APR_EOK == apr_rc ) && 
         ( NULL != psess ) )
    {
      memset( ( void* )psess, 0, sizeof( csd_vm_session_item_t ) );

      apr_rc = apr_objmgr_alloc_object ( &csd_vm_apr_objmgr, &objmgr_obj );
      if ( NULL != objmgr_obj ) 
      {
        objmgr_obj->any.ptr = psess;
        psess->h_apr_obj = objmgr_obj->handle;
      }

      rc = csd_map_apr_error_to_csd( apr_rc );
    }

    if ( ( APR_EOK == apr_rc ) && 
         ( NULL != psess ) )
    {
      apr_rc = csd_event_create ( &psess->csd_vm_event );
      rc = csd_map_apr_error_to_csd( apr_rc );
    }

    if ( ( APR_EOK == apr_rc ) && 
         ( NULL != psess ) )
    {
      switch ( csd_vm_cmd->open_id ) 
      {
        case CSD_VM_OPENID_FULL_CONTROL:
          {
            apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                       (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vm_src_addr,
                                       psess->h_apr_obj,
                                       csd_vm_mvm_dest_addr,
                                       (uint16_t)NULL,
                                       (uint32_t)NULL,
                                       (uint32_t)VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION,
                                       (void*) csd_vm_cmd->u.full_control.name,
                                       (uint32_t)csd_vm_cmd->u.full_control.name_size
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                         "csd_vm_open: VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION sent, apr_rc[0x%x]",
                         apr_rc);

          if ( APR_EOK == apr_rc )
          {
            apr_rc = csd_event_wait ( psess->csd_vm_event );
          }

          if ( APR_EOK != apr_rc )
          {
            CSD_MSG( CSD_PRIO_ERROR,
                  "csd_vm_open: csd_event_wait failed when waiting for Create Full Control, apr_rc[0x%x]",
                  apr_rc);
          }
          rc = csd_map_apr_error_to_csd( apr_rc );

          if ( psess->h_mvm != NULL ) 
          {
            if ( NULL != objmgr_obj ) 
            {
              psess->h_csd = csd_handle_malloc( CSD_OPEN_VOICE_MANAGER, ( void* )&objmgr_obj->handle );
            }

            psess->csd_vm_cb = ( csd_vm_event_callback_fn_t )csd_vm_cmd->u.full_control.callback_fn;
            psess->csd_vm_cb_data = csd_vm_cmd->u.full_control.cb_data;

            strlcpy ( psess->vm_name,
                      csd_vm_cmd->u.full_control.name,
                      sizeof( psess->vm_name )
                     );
            psess->vm_name_size = csd_vm_cmd->u.full_control.name_size;

            csd_vm_update_state (psess, VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION);
          }
          else
          {
            rc = CSD_EFAILED;
            CSD_MSG( CSD_PRIO_ERROR,
                           "csd_vm_open: Unable to get proper handle from CVD" );
          }
        }
        break;
      
        case CSD_VM_OPENID_PASSIVE_CONTROL:
        {
          apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     (uint16_t)NULL,
                                     (uint32_t)NULL,
                                     (uint32_t)VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION,
                                     (void*) csd_vm_cmd->u.passive_control.name,
                                     (uint32_t)csd_vm_cmd->u.passive_control.name_size
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                         "csd_vm_open: VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION sent, apr_rc[0x%x]",
                         apr_rc);
          if ( APR_EOK == apr_rc )
          {
            apr_rc = csd_event_wait ( psess->csd_vm_event );
          }
      
          if ( APR_EOK != apr_rc )
          {
            CSD_MSG( CSD_PRIO_ERROR,
                  "csd_vm_open: csd_event_wait failed when waiting for Create Passive Control, apr_rc[0x%x]",
                  apr_rc);
          }

          if ( psess->h_mvm != NULL )
          {
            if ( NULL != objmgr_obj )
            {
              psess->h_csd = csd_handle_malloc( CSD_OPEN_VOICE_MANAGER, ( void* )&objmgr_obj->handle );
            }

            psess->csd_vm_cb = ( csd_vm_event_callback_fn_t )csd_vm_cmd->u.passive_control.callback_fn;
            psess->csd_vm_cb_data = csd_vm_cmd->u.passive_control.cb_data;

            strlcpy ( psess->vm_name,
                      csd_vm_cmd->u.passive_control.name,
                      sizeof( psess->vm_name )
                     );
            psess->vm_name_size = csd_vm_cmd->u.passive_control.name_size;

            /*update state*/
            psess->is_passive = TRUE;
            csd_vm_update_state (psess, VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION);

            /*call dual control for passive clients VoLTE/CS*/
            dual_control.enable_flag = TRUE;
            apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                         (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                         csd_vm_src_addr,
                                         psess->h_apr_obj,
                                         csd_vm_mvm_dest_addr,                            
                                         psess->h_mvm,
                                         (uint32_t)NULL,
                                         (uint32_t)VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL,
                                         (void*) &dual_control,
                                         (uint32_t)sizeof( vss_imvm_cmd_set_policy_dual_control_t ) );
            CSD_MSG( CSD_PRIO_HIGH,
                         "csd_vm_open: VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL sent, apr_rc[0x%x] enable_flag = [0x%x]",
                         apr_rc, dual_control.enable_flag);
            if ( APR_EOK == apr_rc )
            {
              apr_rc = csd_event_wait ( psess->csd_vm_event );
            }

            if ( APR_EOK != apr_rc )
            {
              CSD_MSG( CSD_PRIO_ERROR,
                    "csd_vm_open: csd_event_wait failed when waiting for Set Policy, apr_rc[0x%x]",
                    apr_rc);
            }
            rc = csd_map_apr_error_to_csd( apr_rc );
          }
          else
          {
            rc = CSD_EFAILED;
            CSD_MSG( CSD_PRIO_ERROR,
                           "csd_vm_open: Unable to get proper handle from CVD" );
          }
        }
        break;
      default:
        rc = CSD_EBADPARAM;
        CSD_MSG( CSD_PRIO_ERROR, "csd_vm_open: Invalid open parameter" );
        break;
      }/* switch statement */
    }/* if ( rc == APR_EOK*/
    
    if ( CSD_EOK == rc ) 
    {
      ( void ) apr_list_add_tail( &csd_vm_active_sess_list, &psess->link );
    }

    if ( ( CSD_EOK != rc ) &&
         ( NULL != objmgr_obj ) ) 
    { /* free resources if mvm did not return a valid handle */
      ( void ) apr_objmgr_free_object ( &csd_vm_apr_objmgr, objmgr_obj->handle );
    }
  }
  *csd_vm_cmd->ret_status = rc;

  if ( ( CSD_EOK == rc ) && ( NULL != psess ) )
  {
    return psess->h_csd;
  }
  else
  {
    return 0;/* return a 0 handle if there is an error */
  }
}

CSD_INTERNAL int32_t csd_vm_close ( uint32_t handle )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vm_session_item_t* psess = NULL;

  psess = ( csd_vm_session_item_t* ) csd_voc_common_obtain_session_from_handle ( handle, &csd_vm_apr_objmgr );

  if ( NULL != psess ) 
  {
    /* Destroy server session */
    if (FALSE == csd_handle_is_ssr(handle)) 
    {
      psess->bclosing_sequence = TRUE;

      /*clean up state properly*/
      if ( psess->sess_state.running )
      {
        /*stop */
        uint32_t cmd_tok = 0;
       rc = csd_vm_ioctl(psess->h_csd,CSD_VM_IOCTL_STOP_VOICE, &cmd_tok, sizeof(cmd_tok) );
        if ( CSD_EOK == rc ) /* what about checking for errors */
        {
          ( void ) csd_event_wait ( psess->csd_vm_event );
        }
        
      }
      if ( psess->sess_state.vs_attached )
      {
        if ( psess->is_passive )
        {
          psess->sess_state.vs_attached = FALSE;
        }
        else
        {
          /*detach VSs*/
          rc = csd_vm_detached_handles(psess, CSD_VSM_VS_HANDLE );
        }
      }
      if (  psess->sess_state.vc_attached )
      {
        /*dettach VCs*/
         rc = csd_vm_detached_handles(psess, CSD_VSM_VC_HANDLE );
      }

      apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                   (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                   csd_vm_src_addr,
                                   (uint16_t)psess->h_apr_obj,
                                   csd_vm_mvm_dest_addr,
                                   (uint16_t)psess->h_mvm,
                                   (uint32_t)NULL,
                                   (uint32_t)APRV2_IBASIC_CMD_DESTROY_SESSION,
                                   NULL, 0 );
      CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_close: APRV2_IBASIC_CMD_DESTROY_SESSION sent, apr_rc[0x%x]",
                       apr_rc);
      if ( APR_EOK == apr_rc ) /* what about checking for errors */
      {
        apr_rc = csd_event_wait ( psess->csd_vm_event );
      }

      if ( APR_EOK != apr_rc )
      {
        CSD_MSG( CSD_PRIO_ERROR,
              "csd_vm_close: csd_event_wait failed when waiting for APR Destroy Session, apr_rc[0x%x]",
              apr_rc);
      }
      rc = csd_map_apr_error_to_csd( apr_rc );

      if( rc == CSD_EOK )
      {
        csd_vm_update_state (psess, APRV2_IBASIC_CMD_DESTROY_SESSION);
      }
    }

    /* free resources associated with session */
    ( void ) csd_event_destroy ( psess->csd_vm_event );
    
    ( void ) csd_handle_free ( psess->h_csd );

    ( void ) apr_objmgr_free_object ( &csd_vm_apr_objmgr, psess->h_apr_obj );

    ( void ) apr_list_delete ( &csd_vm_active_sess_list, &( psess->link ) );

    ( void ) apr_list_add_tail( &csd_vm_free_sess_list, &( psess->link ) );

  }
  else
  {
    rc = CSD_EHANDLE;
  }

  return rc;
}

CSD_INTERNAL int32_t csd_vm_ioctl (
  uint32_t handle,
  uint32_t code,
  void* params,
  uint32_t size )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vm_session_item_t* psess = NULL;
  csd_vm_cmd_list_item_t* pcmd = NULL; /* use this to keep track of the csd vs and vc handle*/

  if ( NULL == params )
  {
    return CSD_EBADPARAM;
  }


  /* extract psession */
  psess = csd_voc_common_obtain_session_from_handle ( handle, &csd_vm_apr_objmgr );

  if ( NULL == psess )
  {
    rc = CSD_EHANDLE;
  }

  if ( CSD_EOK == rc ) 
  {
    switch ( code ) 
    {
      case CSD_VM_IOCTL_ATTACH_STREAM: /* need to get the proper cvs handle */
      {
        csd_vm_ioctl_attach_stream_t* csd_vm_cmd = ( csd_vm_ioctl_attach_stream_t* )params;
        vss_imvm_cmd_attach_stream_t mvm_cmd;
        csd_vs_ioctl_get_cvs_handle_t csd_vm_cvs_ret_handle;

        mvm_cmd.handle = NULL; /* initialize the handle */
  
        csd_vm_cvs_ret_handle.ret_cvs_handle = ( uint16_t* )&mvm_cmd.handle;
  
        rc = csd_vs_ioctl ( csd_vm_cmd->stream_handle,
                            CSD_VS_IOCTL_GET_CVS_HANDLE,
                            &csd_vm_cvs_ret_handle,
                            sizeof ( csd_vs_ioctl_get_cvs_handle_t )
                           );
  
        if ( NULL == mvm_cmd.handle ) 
        {
          rc = CSD_EHANDLE;
          break;
        }

        apr_rc = apr_list_remove_head( &csd_vm_free_cmd_list, ( apr_list_node_t** ) &pcmd );

        if ( ( APR_EOK == apr_rc ) && ( NULL != pcmd ) )
        {
          csd_memscpy ( &pcmd->cmd, sizeof(pcmd->cmd),
                        csd_vm_cmd, sizeof ( csd_vm_ioctl_attach_stream_t ) );
          pcmd->client_token = csd_vm_cmd->cmd_token;
          pcmd->cmd_token = token_counter;
          apr_rc = apr_list_add_tail( &csd_vm_track_dispatched_cmd_list, &pcmd->link );
          apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                       (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vm_src_addr,
                                       (uint16_t)psess->h_apr_obj,
                                       csd_vm_mvm_dest_addr,
                                       psess->h_mvm,
                                       (uint32_t)token_counter++,
                                       (uint32_t)VSS_IMVM_CMD_ATTACH_STREAM,
                                       (void*) &mvm_cmd,
                                       (uint32_t)sizeof(vss_imvm_cmd_attach_stream_t)
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_ATTACH_STREAM sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        }
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_DETACH_STREAM: /* need to get the proper cvs handle */
      {
        csd_vm_ioctl_detach_stream_t* csd_vm_cmd = ( csd_vm_ioctl_detach_stream_t* )params;
        vss_imvm_cmd_detach_stream_t mvm_cmd;
        csd_vs_ioctl_get_cvs_handle_t csd_vm_cvs_ret_handle;
  
        mvm_cmd.handle = NULL; /* initialize the handle */
  
        csd_vm_cvs_ret_handle.ret_cvs_handle = ( uint16_t* )&mvm_cmd.handle;
  
        rc = csd_vs_ioctl ( csd_vm_cmd->stream_handle,
                            CSD_VS_IOCTL_GET_CVS_HANDLE,
                            &csd_vm_cvs_ret_handle,
                            sizeof ( csd_vs_ioctl_get_cvs_handle_t )
                           );
  
        if ( NULL == mvm_cmd.handle ) 
        {
          rc = CSD_EHANDLE;
          break;
        }

        apr_rc = apr_list_remove_head( &csd_vm_free_cmd_list, ( apr_list_node_t** ) &pcmd );

        if ( ( APR_EOK == apr_rc ) && ( NULL != pcmd ) )
        {
          csd_memscpy ( &pcmd->cmd, sizeof(pcmd->cmd),
                       csd_vm_cmd, sizeof ( csd_vm_ioctl_detach_stream_t ) );
          pcmd->cmd_token = token_counter;
          pcmd->client_token = csd_vm_cmd->cmd_token;
          apr_rc = apr_list_add_tail( &csd_vm_track_dispatched_cmd_list, &pcmd->link );
          apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                       ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vm_src_addr,
                                       (uint16_t)psess->h_apr_obj,
                                       csd_vm_mvm_dest_addr,
                                       psess->h_mvm,
                                       ( uint32_t )token_counter++,
                                       ( uint32_t )VSS_IMVM_CMD_DETACH_STREAM,
                                       ( void* ) &mvm_cmd,
                                       ( uint32_t )sizeof( vss_imvm_cmd_detach_stream_t )
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_DETACH_STREAM sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        }
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_ATTACH_CONTEXT:
      {
        csd_vc_ioctl_get_cvp_handle_t cvp_handle;
        vss_imvm_cmd_attach_vocproc_t mvm_cmd;
        csd_vm_ioctl_attach_context_t* csd_vm_cmd = ( csd_vm_ioctl_attach_context_t* )params;
          
        mvm_cmd.handle = NULL;
  
        cvp_handle.ret_cvp_handle = ( uint16_t* )&mvm_cmd.handle;
  
        /* retreive the cvp handle */
        rc = csd_vc_ioctl ( csd_vm_cmd->context_handle,
                            CSD_VC_IOCTL_GET_CVP_HANDLE, 
                            &cvp_handle,
                            sizeof( csd_vc_ioctl_get_cvp_handle_t )
                           );
  
        if ( NULL == mvm_cmd.handle ) 
        {
          rc = CSD_EHANDLE;
          break;
        }

        apr_rc = apr_list_remove_head( &csd_vm_free_cmd_list, ( apr_list_node_t** ) &pcmd );

        if ( ( APR_EOK == apr_rc ) && ( NULL != pcmd ) )
        {
          csd_memscpy ( &pcmd->cmd, sizeof(pcmd->cmd),
                        csd_vm_cmd, sizeof ( csd_vm_ioctl_attach_context_t ) );
          pcmd->client_token = csd_vm_cmd->cmd_token;
          pcmd->cmd_token = token_counter;
          apr_rc = apr_list_add_tail( &csd_vm_track_dispatched_cmd_list, &pcmd->link );
          apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                       (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vm_src_addr,
                                       (uint16_t)psess->h_apr_obj,
                                       csd_vm_mvm_dest_addr,
                                       psess->h_mvm,
                                       (uint32_t)token_counter++,
                                       (uint32_t)VSS_IMVM_CMD_ATTACH_VOCPROC,
                                       (void*) &mvm_cmd,
                                       (uint32_t)sizeof( vss_imvm_cmd_attach_vocproc_t )
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_ATTACH_VOCPROC sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        }  
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_DETACH_CONTEXT:
      {
        csd_vc_ioctl_get_cvp_handle_t cvp_handle;
        vss_imvm_cmd_detach_vocproc_t mvm_cmd;
        csd_vm_ioctl_detach_context_t* csd_vm_cmd = ( csd_vm_ioctl_detach_context_t* )params;
  
        mvm_cmd.handle = NULL;
  
        cvp_handle.ret_cvp_handle = ( uint16_t* )&mvm_cmd.handle;
  
        /* retreive the cvp handle */
        rc = csd_vc_ioctl ( csd_vm_cmd->context_handle,
                            CSD_VC_IOCTL_GET_CVP_HANDLE, 
                            &cvp_handle,
                            sizeof( csd_vc_ioctl_get_cvp_handle_t )
                           );

        if ( NULL == mvm_cmd.handle ) 
        {
          rc = CSD_EHANDLE;
          break;
        }

        apr_rc = apr_list_remove_head( &csd_vm_free_cmd_list, ( apr_list_node_t** ) &pcmd );

        if ( ( APR_EOK == apr_rc ) && ( NULL != pcmd ) )
        {
          csd_memscpy ( &pcmd->cmd, sizeof(pcmd->cmd),
                       csd_vm_cmd, sizeof ( csd_vm_ioctl_detach_context_t ) );
          pcmd->client_token = csd_vm_cmd->cmd_token;
          pcmd->cmd_token = token_counter;
          apr_rc = apr_list_add_tail( &csd_vm_track_dispatched_cmd_list, &pcmd->link );
          apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                       (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vm_src_addr,
                                       (uint16_t)psess->h_apr_obj,
                                       csd_vm_mvm_dest_addr,
                                       psess->h_mvm,
                                       (uint32_t)token_counter++,
                                       (uint32_t)VSS_IMVM_CMD_DETACH_VOCPROC,
                                       (void*) &mvm_cmd,
                                       (uint32_t)sizeof(vss_imvm_cmd_detach_vocproc_t)
                                   );
          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_DETACH_VOCPROC sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        } 
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
/**** Future API for B-family ****/
#if 0
    case CSD_VM_IOCTL_SET_CAL_NETWORK:
      {
        csd_vm_ioctl_set_cal_network_t* csd_vm_cmd = ( csd_vm_ioctl_set_cal_network_t* )params;
        vss_imvm_cmd_set_cal_network_t mvm_cmd;
  
        mvm_cmd.network_id = csd_vm_cmd->network_id;
  
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)csd_vm_cmd->cmd_token,
                                     (uint32_t)VSS_IMVM_CMD_SET_CAL_NETWORK,
                                     (void*) &mvm_cmd,
                                     (uint32_t)sizeof( vss_icommon_cmd_set_network_t )
                                   );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_SET_CAL_NETWORK sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token );
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;

    case CSD_VM_IOCTL_SET_CAL_MEDIA_TYPE:
      {
        csd_vm_ioctl_set_cal_media_t *csd_vm_cmd = ( csd_vm_ioctl_set_cal_media_t* )params;
        csd_vm_ioctl_set_cal_media_t mvm_cmd;
  
        mvm_cmd.media_id = csd_vm_cmd->media_id;
  
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)csd_vm_cmd->cmd_token,
                                     (uint32_t)VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE,
                                     (void*) &mvm_cmd,
                                     (uint32_t)sizeof( vss_imvm_cmd_set_cal_media_type_t )
                                   );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token );
		rc = csd_map_apr_error_to_csd( apr_rc );
  
      }
      break;
#endif

      case CSD_VM_IOCTL_SET_NETWORK:
      {
        csd_vm_ioctl_set_network_t* csd_vm_cmd = ( csd_vm_ioctl_set_network_t* )params;
        vss_icommon_cmd_set_network_t mvm_cmd;
  
        mvm_cmd.network_id = csd_vm_cmd->network_id;
  
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)csd_vm_cmd->cmd_token,
                                     (uint32_t)VSS_ICOMMON_CMD_SET_NETWORK,
                                     (void*) &mvm_cmd,
                                     (uint32_t)sizeof( vss_icommon_cmd_set_network_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_ICOMMON_CMD_SET_NETWORK sent, apr_rc[0x%x] token[0x%x] , network_id[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token, mvm_cmd.network_id);
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_SET_VOICE_TIMING:
      {
        csd_vm_ioctl_set_voice_timing_t* csd_vm_cmd = ( csd_vm_ioctl_set_voice_timing_t* )params;
        vss_icommon_cmd_set_voice_timing_t mvm_cmd;
  
        mvm_cmd.mode = csd_vm_cmd->mode;
        mvm_cmd.enc_offset = csd_vm_cmd->enc_offset;
        mvm_cmd.dec_req_offset = csd_vm_cmd->dec_req_offset;
        mvm_cmd.dec_offset = csd_vm_cmd->dec_offset;

        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)csd_vm_cmd->cmd_token,
                                     (uint32_t)VSS_ICOMMON_CMD_SET_VOICE_TIMING,
                                     (void*) &mvm_cmd,
                                     (uint32_t)sizeof( vss_icommon_cmd_set_voice_timing_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_ICOMMON_CMD_SET_VOICE_TIMING sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_SET_TTY_MODE:
      {
        csd_vm_ioctl_set_tty_mode_t* csd_vm_cmd = ( csd_vm_ioctl_set_tty_mode_t* )params;
        vss_itty_cmd_set_tty_mode_t mvm_cmd;

        mvm_cmd.mode = csd_vm_cmd->mode;

        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)csd_vm_cmd->cmd_token,
                                     (uint32_t)VSS_ITTY_CMD_SET_TTY_MODE,
                                     (void*) &mvm_cmd,
                                     (uint32_t)sizeof( vss_itty_cmd_set_tty_mode_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_ITTY_CMD_SET_TTY_MODE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_SET_WIDEVOICE:
      {
        csd_vm_ioctl_set_widevoice_t* csd_vm_cmd = ( csd_vm_ioctl_set_widevoice_t* )params;
        vss_iwidevoice_cmd_set_widevoice_t mvm_cmd;

        mvm_cmd.enable = csd_vm_cmd->enable;

        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)csd_vm_cmd->cmd_token,
                                     (uint32_t)VSS_IWIDEVOICE_CMD_SET_WIDEVOICE,
                                     (void*) &mvm_cmd,
                                     (uint32_t)sizeof( vss_iwidevoice_cmd_set_widevoice_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IWIDEVOICE_CMD_SET_WIDEVOICE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vm_cmd->cmd_token);
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_START_VOICE:
      {
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)(* ( uint32_t* )params ),/* the input parameter for this command is the cmd_token */
                                     (uint32_t)VSS_IMVM_CMD_START_VOICE,
                                     NULL,
                                     0
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vm_ioctl: VSS_IMVM_CMD_START_VOICE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, (uint32_t)(* ( uint32_t* )params ));
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;

      case CSD_VM_IOCTL_STOP_VOICE:
      {
        /*store vp3 data*/
        csd_vm_store_vp3_data(psess);
        
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,  
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)(* ( uint32_t* )params ),/* the input parameter for this command is the cmd_token */
                                     (uint32_t)VSS_IMVM_CMD_STOP_VOICE,
                                     NULL,
                                     0
                                   );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vm_ioctl: VSS_IMVM_CMD_STOP_VOICE sent, apr_rc[0x%x]", apr_rc );
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_STANDBY_VOICE:
      {
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)(* ( uint32_t* )params ),/* the input parameter for this command is the cmd_token */
                                     (uint32_t)VSS_IMVM_CMD_STANDBY_VOICE,
                                     NULL,
                                     0
                                   );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vm_ioctl: VSS_IMVM_CMD_STANDBY_VOICE sent, apr_rc[0x%x]", apr_rc );
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      break;
      
      case CSD_VM_IOCTL_PAUSE_VOICE:
      {
        /*store vp3 data*/
        csd_vm_store_vp3_data(psess);
      
        apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vm_src_addr,
                                     (uint16_t)psess->h_apr_obj,
                                     csd_vm_mvm_dest_addr,
                                     psess->h_mvm,
                                     (uint32_t)(* ( uint32_t* )params ),/* the input parameter for this command is the cmd_token */
                                     (uint32_t)VSS_IMVM_CMD_PAUSE_VOICE,
                                     NULL,
                                     0
                                   );
        CSD_MSG( CSD_PRIO_HIGH, "csd_vm_ioctl: VSS_IMVM_CMD_PAUSE_VOICE sent, apr_rc[0x%x]", apr_rc );
        rc = csd_map_apr_error_to_csd( apr_rc ); 
      }
      break;

      case CSD_VM_IOCTL_SET_HDVOICE_MODE:
      {
        csd_vm_ioctl_set_hdvoice_mode_t* cmd = ( csd_vm_ioctl_set_hdvoice_mode_t* )params;
        uint32_t mvm_api = 0;

        switch( cmd->mode )
        {
          case CSD_VM_IOCTL_HDVOICE_OFF:
            mvm_api = VSS_IHDVOICE_CMD_DISABLE;
          break;
          
          case CSD_VM_IOCTL_HDVOICE_ON:
            mvm_api = VSS_IHDVOICE_CMD_ENABLE;
          break;
          
          case CSD_VM_IOCTL_HDVOICE_ON_WV2_OFF:  
            apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                         csd_vm_src_addr, psess->h_apr_obj,
                                         csd_vm_mvm_dest_addr, psess->h_mvm,
                                         cmd->cmd_token, VSS_IHDVOICE_CMD_WV2_DISABLE,
                                         NULL, 0 ); 
			
            CSD_MSG( CSD_PRIO_HIGH, "csd_vm_ioctl: VSS_IHDVOICE_CMD_WV2_DISABLE sent, apr_rc[0x%x]", apr_rc );
            if( APR_EOK == apr_rc )
            {
              apr_rc = csd_event_wait( psess->csd_vm_event );
            }
            if ( APR_EOK != apr_rc )
            {
              CSD_MSG( CSD_PRIO_ERROR,
                    "csd_vm_ioctl: csd_event_wait failed when waiting for HDon WV2 off, apr_rc[0x%x]",
                    apr_rc);
            }
            rc = csd_map_apr_error_to_csd( apr_rc );
            
            mvm_api = VSS_IHDVOICE_CMD_ENABLE;
          break;
          
          case CSD_VM_IOCTL_HDVOICE_ON_BEAMR_OFF:
            apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                         csd_vm_src_addr, psess->h_apr_obj,
                                         csd_vm_mvm_dest_addr, psess->h_mvm,
                                         cmd->cmd_token, VSS_IHDVOICE_CMD_BEAMR_DISABLE,
                                         NULL, 0 ); 
			
            CSD_MSG( CSD_PRIO_HIGH, "csd_vm_ioctl: VSS_IHDVOICE_CMD_BEAMR_DISABLE sent, apr_rc[0x%x]", apr_rc );	
			
            if( APR_EOK == apr_rc )
            {
              apr_rc = csd_event_wait( psess->csd_vm_event );
            }
            if ( APR_EOK != apr_rc )
            {
              CSD_MSG( CSD_PRIO_ERROR,
                    "csd_vm_ioctl: csd_event_wait failed when waiting for HDon BeAMR off, apr_rc[0x%x]",
                    apr_rc);
            }
            rc = csd_map_apr_error_to_csd( apr_rc );
            
            mvm_api = VSS_IHDVOICE_CMD_ENABLE;
          break;

          default:
            CSD_MSG( CSD_PRIO_ERROR, "csd_vm_ioctl: CSD_VM_IOCTL_SET_HDVOICE_MODE unknown mode[%d]", cmd->mode );
            rc = CSD_EBADPARAM;
          break;
        }
        
        if( CSD_EOK == rc )
        {
          apr_rc = __aprv2_cmd_alloc_send( csd_vm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                           csd_vm_src_addr, psess->h_apr_obj,
                                           csd_vm_mvm_dest_addr, psess->h_mvm,
                                           cmd->cmd_token, mvm_api,
                                           NULL, 0 ); 
            
          CSD_MSG( CSD_PRIO_HIGH, "csd_vm_ioctl: API[0x%x] sent, apr_rc[0x%x]", mvm_api, apr_rc );
          rc = csd_map_apr_error_to_csd( apr_rc ); 
        }
      }
      break;

      default: /* All others not supported for FC1 */
        rc = CSD_EUNSUPPORTED;
        CSD_MSG( CSD_PRIO_ERROR, "csd_vm_ioctl: Unsupported opcode[0x%x]", code );
      break;
    } /* switch cmd */
  }/* if rc == CSD_EOK */

  return rc;
}

CSD_INTERNAL int32_t csd_vm_read (
  uint32_t handle,
  void* params,
  uint32_t size
)
{
  return CSD_EFAILED;
}

CSD_INTERNAL int32_t csd_vm_write (
  uint32_t handle,
  void* params,
  uint32_t size
)
{
  return CSD_EFAILED;
}



/*
 ** End Internal functions
*/

