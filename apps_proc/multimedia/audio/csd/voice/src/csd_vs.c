/*-----------------------------------------------------------------------------
     Copyright (c) 2012, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
   Qualcomm Confidential and Proprietary
  -----------------------------------------------------------------------------*/
  /*=============================================================================
                      EDIT HISTORY FOR MODULE
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csd/voice/src/csd_vs.c#2 $
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
  10/02/14      jk       dtmf end and generation fix
  09/01/14      rp       enhanced the csd init debug buffer 
  08/25/14      jk       Added per vocoder calibration support 
  04/16/14      jk       added support for Voice Session Manager
  02/06/14      vk       modified code for proper csd/acdb/apr error mapping
  07/03/13      vk       Porting changes related to CR499559 and
                         Porting changes from mainline code 
  06/05/13      aim      Added support for unified CSD msg logging.
  12/06/12      rp       updated csd init debug info for 9x25
  07/18/12      jy       added support for 9x25 outband calibration
  05/25/12      rp       Added support for Outband calibration
  12/08/11      sr       Added Mute_V2 implemenattion. 
  12/06/11      jk       added support for passive control handle storage for RTC
  12/06/11      jk       added support for CSD_VS_IOCTL_START_RECORD_V2 and 
                         CSD_VS_IOCTL_START_PLAYBACK_V2 ioctls 

=============================================================================*/
#include "csd_vs_private.h"
#include "csd_debug_info.h"
#include "csd_os_dependencies.h"
#include "csd_error_map.h"
/*
 ** Start Internal Data & Structures
*/
/* Variables for APR */
static uint32_t csd_vs_apr_handle;
static uint8_t csd_vs_cvs_dest_dns[] = "qcom.adsp.cvs";
static uint8_t csd_vs_mvm_dest_dns[] = "qcom.adsp.mvm";
static char_t csd_vs_dns[] = "qcom.apps.csd_vs";
static uint16_t csd_vs_src_addr;
static uint16_t csd_vs_cvs_dest_addr;
static uint16_t csd_vs_mvm_dest_addr;

/* Lock management */
static csd_lock_t csd_vs_task_lock;

/* Variables to keep track of command items */
static apr_list_t csd_vs_free_cmd_list;
static apr_list_t csd_vs_active_cmd_list;
static apr_list_t csd_vs_track_dispatched_cmd_list; /* this keeps cmd sent for open + close that are awaiting a response */
static csd_vs_cmd_list_item_t csd_vs_cmd_pool[ CSD_VS_NUM_COMMANDS ];

/* Variables to keep track of session items */
static apr_list_t csd_vs_free_sess_list;
static apr_list_t csd_vs_active_sess_list;
static csd_vs_session_item_t csd_vs_sess_pool[ CSD_VS_NUM_SESSIONS ];

/* Variables used for APR Object Management */
static apr_objmgr_object_t csd_vs_object_table[ CSD_VS_MAX_OBJECTS_V ];
static apr_objmgr_t csd_vs_apr_objmgr;

static struct csd_vtable csd_vs_vtable; /* CSD Function Table */

static uint32_t token_counter = 0; /* Unique Token counter to keep track of GET_UI cmds sent */
static uint32_t h_csd_voc_common;  /* Handle to csd voice common */

static uint8_t* csd_vs_va_cal = NULL;
static uint64_t csd_vs_pa_cal = 0;
static uint8_t* csd_vs_pVAMemoryTable = NULL;
static uint32_t csd_vs_mapped_pmem_handle = 0;
static uint32_t csd_vs_cal_buf_size = 0;
static uint32_t csd_vs_cal_size = 0;
static uint32_t csd_vs_memory_blocks = 0;
static void* table_memory_handle = NULL;
static void* buffer_memory_handle = NULL;


/*
 ** End Internal Data Structures
*/

/*
 ** Start Internal functions
*/

static void csd_vs_task_lock_fn ( void )
{
  ( void ) csd_lock_enter( csd_vs_task_lock );
}

static void csd_vs_task_unlock_fn ( void )
{
  ( void ) csd_lock_leave( csd_vs_task_lock );
}

int32_t csd_vs_send_cal_fn( csd_vs_session_item_t  *psess )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint32_t csd_vs_cal_column_info_size;
  vss_istream_cmd_register_static_calibration_data_t csd_vs_cal_cmd;
  aprv2_packet_t* papr_packet = NULL;

  if ( NULL != psess )
  {
    if ( FALSE == psess->b_calibration_done )
    {
      uint32_t cal_cmd = VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA;        
     
      csd_vs_cal_cmd.num_columns = 0; /* initialize here to avoid klocwork issues */
      csd_vs_cal_cmd.cal_mem_handle = csd_vs_mapped_pmem_handle;
      csd_vs_cal_cmd.cal_mem_size = csd_vs_cal_size;

      if ( csd_vs_memory_blocks > 1 )
      { /* if the memory is non-contiguous CVD expects an offset */
        csd_vs_cal_cmd.cal_mem_address = 0;
      }
      else
      { /* if the memory is contiguous CVD expects a physical address */
        csd_vs_cal_cmd.cal_mem_address = ( uint64_t )csd_vs_pa_cal;
      }

      /* Create APR Packet with space for Column Info*/
      csd_vs_cal_column_info_size = csd_voc_common_get_cal_column_info_size ( CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO_SIZE, NULL, NULL );

      apr_rc = __aprv2_cmd_alloc_ext( csd_vs_apr_handle,
                                ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                csd_vs_src_addr,
                                ( uint16_t )psess->h_apr_obj,
                                csd_vs_cvs_dest_addr,
                                psess->h_cvs,
                                psess->set_dev_token,
                                cal_cmd,
                                sizeof( vss_istream_cmd_register_static_calibration_data_t ) + csd_vs_cal_column_info_size,
                                &papr_packet );

      if( ( APR_EOK == apr_rc ) && ( papr_packet != NULL ) )
      { /* Copy Column Info into APR packet */
        uint8_t* pdata = NULL;
        uint32_t psize = 0;
        uint32_t column_write_size = 0;

        pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
        psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(papr_packet->header);
        csd_memscpy( pdata, psize,
                     &csd_vs_cal_cmd, sizeof ( vss_istream_cmd_register_static_calibration_data_t ) ); /* copy the cmd structure */
        pdata = pdata + sizeof( vss_istream_cmd_register_static_calibration_data_t ) - sizeof ( uint32_t );

        csd_voc_common_get_cal_column_info ( CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO,
                                            pdata,
                                            csd_vs_cal_column_info_size + sizeof ( uint32_t ),
                                            &column_write_size,
                                             NULL,
                                             NULL);
        if ( column_write_size )
        {
          apr_rc = __aprv2_cmd_forward( csd_vs_apr_handle, papr_packet );
        }
        else
        {
          apr_rc = __aprv2_cmd_free( csd_vs_apr_handle, papr_packet );
        }
      }
    }
  }

  rc = csd_map_apr_error_to_csd ( apr_rc );
  return rc;
}

uint32_t csd_vs_update_state (csd_vs_session_item_t  *psess, uint32_t cmd)
{
  uint32_t rc = CSD_EOK;
  switch( psess->sess_state.state )
  {
    case CSD_VSM_HANDLE_STATE_UNINITIALIZED:
      /* if session is opened transition*/
      if( cmd == VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION )
      {
        psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;

        /*set media type flag if params were passed in open*/
        if ( psess->enc_type != 0 && psess->dec_type != 0 )
        {
          psess->sess_state.vs_media_type_set = TRUE;
        }
        rc = csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );

      }
      /* if passive move from open directly to ready and assume vs params are set */
      if ( cmd == VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          rc = csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );
          
           psess->sess_state.vs_media_type_set = TRUE;
           psess->sess_state.vs_rate_set = TRUE;
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_READY;
          rc = csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );
      }
      break;
    case CSD_VSM_HANDLE_STATE_OPEN:
       /* if session is opened transition*/
      if( cmd == APRV2_IBASIC_CMD_DESTROY_SESSION )
      {
         psess->sess_state.state = CSD_VSM_HANDLE_STATE_UNINITIALIZED;
         rc = csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );
      }
      else
      {
        if( psess->sess_state.vs_media_type_set && psess->sess_state.vs_rate_set )
        {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_READY;
         rc =  csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );
        }
      }
      break;
    case CSD_VSM_HANDLE_STATE_READY:
       /* if session is opened transition*/
      if( cmd == APRV2_IBASIC_CMD_DESTROY_SESSION )
      {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_UNINITIALIZED;
          rc = csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );
      }
      else
      {
        if( !psess->sess_state.vs_media_type_set || !psess->sess_state.vs_rate_set )
        {
          psess->sess_state.state = CSD_VSM_HANDLE_STATE_OPEN;
          rc = csd_vsm_update_state( psess->h_csd, psess->vs_name, psess->sess_state.state );
        }
      }
      break;
    default:
       CSD_MSG( CSD_PRIO_ERROR, "csd_vs_update_state: Unknown state[%d]", psess->sess_state.state );
       rc = CSD_EBADSTATE;
  }
  return rc;
}

bool_t csd_vs_session_exsists(const char* session_name)
{
  bool_t rc = FALSE;
  csd_vs_session_item_t *link;
  csd_vs_session_item_t *pivot;
   
  /* check if the vs is belonged to this vm */
  if ( !( apr_list_peak_head ( &csd_vs_active_sess_list, ( apr_list_node_t ** )&link) ) )
  {    
    do 
    {
      if ( strncmp( session_name, link->vs_name, link->vs_name_size )== 0 )
      {
        rc = TRUE;
        break;
      }
      pivot = link;
    } while ( ! ( apr_list_get_next ( &csd_vs_active_sess_list,
                                      ( apr_list_node_t* )pivot, ( apr_list_node_t** )&link ) ));
  }
  return rc;
}

uint32_t csd_vs_command_fn( uint32_t param)
{
  int32_t rc = CSD_EOK;
  csd_vs_cmd_list_item_t* pcmd = NULL;

  while ( ! ( apr_list_remove_head( &csd_vs_active_cmd_list, ( apr_list_node_t** )&pcmd ) ) )
  {
    if ( pcmd != NULL )
    {
      switch ( pcmd->type )
      {
        case CSD_VS_CMD_SEND_RSP_CALLBACK:
        {
          csd_vs_event_command_rsp_t event_rsp;

          if ( pcmd->psess->csd_vs_cb )
          {
            event_rsp.status_id = pcmd->cmd.rsp_cmd.status_id;

            pcmd->psess->csd_vs_cb ( pcmd->client_token,
                                     CSD_VS_EVENTID_COMMAND_RSP,
                                     &event_rsp,
                                     sizeof ( event_rsp ),
                                     pcmd->psess->csd_vs_cb_data );

            CSD_MSG( CSD_PRIO_LOW,
                           "csd_vs_command_fn: CSD_VS_CMD_SEND_RSP_CALLBACK , token[0x%x], result[0x%x]",
                           pcmd->client_token,
                           pcmd->cmd.rsp_cmd.status_id );

          }
          else
          {
            CSD_MSG( CSD_PRIO_LOW,
                          "csd_vs_command_fn: CSD_VS_CMD_SEND_RSP_CALLBACK , no callback has been registered!" );
          }
        }
        break;

        case CSD_VS_CMD_SEND_STATE_NOTIFY_CALLBACK:
        {
          csd_vs_event_state_notify_t event_state;

          if ( pcmd->psess->csd_vs_cb )
          {
            event_state.state_id = pcmd->cmd.state_cmd.state_id;

            pcmd->psess->csd_vs_cb ( pcmd->client_token,
                                     CSD_VS_EVENTID_STATE_NOTIFY,
                                     &event_state,
                                     sizeof ( event_state ),
                                     pcmd->psess->csd_vs_cb_data );

            CSD_MSG( CSD_PRIO_LOW,
                           "csd_vs_command_fn: CSD_VS_CMD_SEND_STATE_NOTIFY_CALLBACK , token[0x%x], state[0x%x]",
                           pcmd->client_token,
                           pcmd->cmd.state_cmd.state_id );
          }
        }
        break;

        case CSD_VS_CMD_SEND_CALIBRATION:
        {
          rc = csd_vs_send_cal_fn ( pcmd->psess );
        }
        break;

        case CSD_VS_CMD_SEND_DEC_BUF_REQ_CALLBACK:
        {
          if ( pcmd->psess->csd_vs_cb )
          {
            pcmd->psess->csd_vs_cb ( 0,
                                     CSD_VS_EVENTID_SIGNAL_DEC_BUFFER,
                                     NULL,
                                     0,
                                     pcmd->psess->csd_vs_cb_data);
          }
        }
        break;

        case CSD_VS_CMD_SEND_ENC_BUF_READY_CALLBACK:
        {
          if ( pcmd->psess->csd_vs_cb )
          {
            pcmd->psess->csd_vs_cb ( 0,
                                     CSD_VS_EVENTID_SIGNAL_ENC_BUFFER,
                                     NULL,
                                     0,
                                     pcmd->psess->csd_vs_cb_data);
          }
        }
        break;

      case CSD_VS_CMD_UPDATE_STATE:
        csd_vs_update_state(pcmd->psess, pcmd->cmd.update_state_cmd);
        break;

      case CSD_VS_CMD_SEND_DTMF_ENDED:
      {
        if ( NULL != pcmd->cmd.dtmf_end && pcmd->psess->csd_vs_cb )
        {
          pcmd->psess->csd_vs_cb ( pcmd->client_token, CSD_VS_EVENTID_DTMF_GENERATION_ENDED,
                               pcmd->cmd.dtmf_end, sizeof ( csd_vs_event_dtmf_generation_ended_t ), pcmd->psess->csd_vs_cb_data );
        }
      }
      break;

      case CSD_VS_CMD_SEND_DTMF_DETECTED:
      {
        if ( NULL != pcmd->cmd.dtmf_detected && pcmd->psess->csd_vs_cb )
        {
          pcmd->psess->csd_vs_cb( pcmd->client_token, CSD_VS_EVENTID_RX_DTMF_DETECTED,
                               pcmd->cmd.dtmf_detected, sizeof ( csd_vs_event_rx_dtmf_detected_t ), pcmd->psess->csd_vs_cb_data );
        }
      }
      break;

      case CSD_VS_COMMAND_MAX:
        break;
      } /* End of Switch */
    }
    ( void ) apr_list_add_tail( &csd_vs_free_cmd_list, &pcmd->link );

  } /* End while cmds */

  return rc;
}

static void csd_vs_get_cmd_node ( uint32_t cmd_token, csd_vs_cmd_list_item_t **pcmd_node )
{
  csd_vs_cmd_list_item_t* pcmd_pivot = NULL;
  csd_vs_cmd_list_item_t* pcmd = NULL;

  *pcmd_node = NULL;

  if ( !( apr_list_peak_head ( &csd_vs_track_dispatched_cmd_list, ( apr_list_node_t ** )&pcmd ) ) )
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

    } while ( ! ( apr_list_get_next ( &csd_vs_track_dispatched_cmd_list,
              ( apr_list_node_t* )pcmd_pivot, ( apr_list_node_t** )&pcmd ) ) );
  }
}

static int32_t csd_vs_apr_callback (
  aprv2_packet_t* packet,
  void* dispatch_data
){
  csd_voc_common_sig_enum_t sigs  = CSD_VOC_COMMON_SIG_NONE;
  aprv2_ibasic_rsp_result_t  *rsp;
  csd_vs_session_item_t  *psess = NULL;
  apr_objmgr_object_t* apr_obj = NULL;

  if ( NULL != packet )
  {
    ( void ) apr_objmgr_find_object ( &csd_vs_apr_objmgr, packet->dst_port, &apr_obj );

    if ( apr_obj != NULL )
    {
      psess = apr_obj->any.ptr;

      if( NULL != psess )
      {
        switch( packet->opcode )
        {
        case APRV2_IBASIC_RSP_RESULT:
        {
          rsp = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet);

          if ( NULL != rsp)
          {
            switch( rsp->opcode )
            {
            case VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION:
            case VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION:
            {
              csd_vs_cmd_list_item_t* pcmd = NULL;

              psess->h_cvs = packet->src_port;

              if( csd_vs_cal_buf_size == 0 )
              {
                CSD_MSG( CSD_PRIO_HIGH, "csd_vs_apr_callback: VSS_ISTREAM_CMD_CREATE_FULL/PASSIVE_CONTROL_SESSION returned, apr_rc[0x%x],"
                         "WILL NOT SEND VOICE CALIBRATION DATA", rsp->status );
                csd_event_signal ( psess->csd_vs_event );
              }
              else
              {
                if ( APR_EOK == rsp->status )
                { /* if we get a successful creation then start caching calibration */
                /* Signal CSD Voc Common Thread to initiate calibration caching */

                ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
                  if ( NULL != pcmd )
                  {
                        // send calibration only if never did before
                    pcmd->type = CSD_VS_CMD_SEND_CALIBRATION;
                    pcmd->psess = psess;
                    pcmd->client_token = NULL;

                    ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
                    /* signal the csd common thread to process the cmd */
                    sigs |= CSD_VOC_COMMON_SIG_CMD;
                    ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
                  }
                }
                else
                { /* if the creation was not successful then signal the event to bring csd_vs_open out of waiting */
                  ( void ) csd_event_signal ( psess->csd_vs_event );
                }
              }
            }
            break;

            case APRV2_IBASIC_CMD_DESTROY_SESSION:
              csd_event_signal ( psess->csd_vs_event ); /* we need to keep track of the tokens */
            break;

            case VSS_ISTREAM_CMD_SET_MEDIA_TYPE:
            case VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE:
            case VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION:
            case VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE:
            case VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE:
            case VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE:
            case VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE:
            case VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE:
            {
              csd_vs_cmd_list_item_t* pcmd = NULL;
              csd_vs_cmd_list_item_t* pcmd2 = NULL;

              if ( rsp->status == APR_EOK )
              {
                if ( rsp->opcode == VSS_ISTREAM_CMD_SET_MEDIA_TYPE )
                {
                   psess->sess_state.vs_media_type_set = TRUE;
                }
                else
                {
                   psess->sess_state.vs_rate_set = TRUE;
                }

                 /*send update*/
                ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, (apr_list_node_t**)&pcmd2 );
                if( pcmd2 )
                {
                    pcmd2->type = CSD_VS_CMD_UPDATE_STATE;
                    pcmd2->cmd.update_state_cmd = rsp->opcode;
                    pcmd2->psess = psess;
                    ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd2->link );
                    /* signal the csd common thread to process the cmd */
                    sigs |= CSD_VOC_COMMON_SIG_CMD;
                    ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs );
                }
              }

              /* obtain a free command item */
              ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
              if ( pcmd != NULL)
              {

                pcmd->type = CSD_VS_CMD_SEND_RSP_CALLBACK;
                pcmd->psess = psess;
                pcmd->cmd.rsp_cmd.status_id= rsp->status;
                pcmd->client_token = packet->token;

                ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
                /* signal the csd common thread to process the cmd */
                sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
              }
            }
            break;
            case VSS_ISTREAM_CMD_SET_ENC_DTX_MODE:
            case VSS_ISTREAM_CMD_SET_DEC_TIMEWARP:
            case VSS_ISTREAM_CMD_SET_DTMF_GENERATION:
            case VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION:
            case VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE:
            case VSS_ICOMMON_CMD_SET_UI_PROPERTY:
            case VSS_IVOLUME_CMD_MUTE_V2:
            case VSS_ISTREAM_CMD_START_RECORD:
            case VSS_ISTREAM_CMD_START_PLAYBACK:
            case VSS_IPLAYBACK_CMD_START:
            case VSS_IPLAYBACK_CMD_STOP:
            case VSS_IRECORD_CMD_START:
            case VSS_IRECORD_CMD_STOP:
            {
              csd_vs_cmd_list_item_t* pcmd = NULL;

              /* obtain a free command item */
              ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
              if ( pcmd != NULL)
              {
                pcmd->type = CSD_VS_CMD_SEND_RSP_CALLBACK;
                pcmd->psess = psess;
                pcmd->cmd.rsp_cmd.status_id= rsp->status;
                pcmd->client_token = packet->token;

                ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
                /* signal the csd common thread to process the cmd */
                sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
              }
            }
            break;

            case VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE:
            {
              /*  Signal csd_vs_open() to continue OoB VoIP Setup  */
              csd_event_signal ( psess->csd_vs_event );
              if ( APR_EOK == rsp->status )
              {
                CSD_MSG(CSD_PRIO_LOW, "csd_vs_apr_callback: Set Packet Exchange Mode Successful");
              }
              else
              {
                CSD_MSG(CSD_PRIO_ERROR, "csd_vs_apr_callback: Unable to set Packet exchange to OoB, status[0x%x]", rsp->status);
              }
            }
            break;

            case VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG:
            {
              /*  Bring csd_vs_open out of wait state when in full control for VoIP  */
              csd_event_signal ( psess->csd_vs_event );

              if ( APR_EOK == rsp->status )
              {
                CSD_MSG(CSD_PRIO_LOW, "csd_vs_apr_callback: OoB Packet Exchange Config successful");
              }
              else
              {
                CSD_MSG(CSD_PRIO_ERROR, "csd_vs_apr_callback: Error on OoB Packet Exchange config return[0x%x]", rsp->status);
              }
            }
            break;

            case VSS_IMEMORY_CMD_MAP_PHYSICAL: /* this is an error case */
            {
              /* Signal thread in csd_vs_init() out of wait state */
              ( void ) csd_event_signal ( psess->csd_vs_event );

              CSD_MSG(CSD_PRIO_LOW,
                          "csd_vs_apr_callback: VSS_IMEMORY_CMD_MAP_PHYSICAL failed" );
            }
            break;

            case VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2:
            case VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA:
            {
              if ( rsp->status == APR_EOK || rsp->status == APR_EALREADY )
              {
                /* set calibration to done */
                psess->b_calibration_done = TRUE;
              }

              /* set event to remove csd_vs_open() out of wait state */
              ( void ) csd_event_signal ( psess->csd_vs_event );
            }
            break;

            case VSS_ISTREAM_CMD_DEREGISTER_CALIBRATION_DATA:
            case VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA:
            {
              psess->b_calibration_done = FALSE;

              if ( rsp->status != APR_EOK )
              {
                /* set calibration to done */
                CSD_MSG(CSD_PRIO_LOW,
                                "csd_vs_apr_callback: VSS_ISTREAM_CMD_DEREGISTER_CALIBRATION_DATA failed" );
              }

              /* Signal csd_vs_close() thread out of wait state */
              ( void ) csd_event_signal ( psess->csd_vs_event );
            }
            break;

            case VSS_IMEMORY_CMD_UNMAP:
            {
              if ( rsp->status != APR_EOK )
              {
                /* set calibration to done */
                CSD_MSG( CSD_PRIO_LOW,
                               "csd_vs_apr_callback: VSS_IMEMORY_CMD_UNMAP failed" );
              }

              /* Signal thread in csd_vs_deinit() out of wait state */
              ( void ) csd_event_signal ( psess->csd_vs_event );
            }
            break;

            case VSS_ICOMMON_RSP_GET_UI_PROPERTY:
            { /* we need to keep track of the the user's buffer */
              csd_vs_cmd_list_item_t* pcmd = NULL; /* pointer to tracked command item */
              csd_vs_cmd_list_item_t* pcmd_cb = NULL; /* pointer to callback command item*/
              vss_icommon_rsp_get_ui_property_t *get_ui_rsp = NULL;
              /* retreive the client's command request based on unique token */
              ( void )csd_vs_get_cmd_node ( packet->token, &pcmd );

              get_ui_rsp = ( vss_icommon_rsp_get_ui_property_t * )APRV2_PKT_GET_PAYLOAD( uint8_t, packet );

              if ( ( NULL != pcmd) &&
                   ( NULL != get_ui_rsp ) &&
                   ( 0 == get_ui_rsp->status ) &&
                   ( NULL != pcmd->cmd.getui_cmd.param_data ) &&
                   ( get_ui_rsp->module_id == pcmd->cmd.getui_cmd.module_id ) &&
                   ( get_ui_rsp->param_id == pcmd->cmd.getui_cmd.param_id ) &&
                   ( get_ui_rsp->param_size <= pcmd->cmd.getui_cmd.param_size ) )
              {
                csd_memscpy( pcmd->cmd.getui_cmd.param_data,
                             pcmd->cmd.getui_cmd.param_size,
                             get_ui_rsp + sizeof ( vss_icommon_rsp_get_ui_property_t ),/* refer to the vss_common_public_if.h */
                             get_ui_rsp->param_size );
              }
              /* return the tracked cmd item back to free queue */
              if ( NULL != pcmd )
              { /* return the list item back into the free cmd list */
                ( void ) apr_list_delete ( &csd_vs_track_dispatched_cmd_list, ( apr_list_node_t* )&pcmd->link );
                ( void ) apr_list_add_tail ( &csd_vs_free_cmd_list, ( apr_list_node_t* )&pcmd->link );
              }

              csd_event_signal ( psess->csd_vs_event ); /* Bring GET UI Call Out of Wait State */

              /* obtain a free command item to send response to csd client */
              ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd_cb ) );

              if ( pcmd_cb != NULL)
              {
                pcmd_cb->type = CSD_VS_CMD_SEND_RSP_CALLBACK;
                pcmd_cb->psess = psess;
                pcmd_cb->cmd.rsp_cmd.status_id= rsp->status;
                if ( NULL != pcmd )
                {
                  pcmd_cb->client_token = pcmd->client_token; /* bug!!!*/ /* this is the original client token and not the token used for apr */
                }

                ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd_cb->link );
                /* signal the csd common thread to process the cmd */
                sigs |= CSD_VOC_COMMON_SIG_CMD;
                ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
              }
            }
            break;

            default:
              CSD_MSG(CSD_PRIO_LOW,
                          "csd_vs_apr_callback: unsupported rsp opcode" );
            break;
            }/* switch( rsp->opcode ) */
          } /* if ( NULL != rsp) */
        }
        break;

        case VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED:
        {
          csd_vs_cmd_list_item_t* pcmd = NULL; /* pointer to tracked command item */
          /* obtain a free command item */
          ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
          if ( pcmd != NULL )
          {
            pcmd->type = CSD_VS_CMD_SEND_DTMF_ENDED;
            pcmd->cmd.dtmf_end = APRV2_PKT_GET_PAYLOAD( csd_vs_event_dtmf_generation_ended_t, packet);
            pcmd->client_token = packet->token;
			pcmd->psess = psess;
            ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
            /* signal the csd common thread to process the cmd */
            sigs |= CSD_VOC_COMMON_SIG_CMD;
            ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
          }
          CSD_MSG(CSD_PRIO_LOW,
                        "csd_vs_apr_callback: DTMF Generation eneded request" );

        }
        break;

        case VSS_ISTREAM_EVT_RX_DTMF_DETECTED:
        {
          csd_vs_cmd_list_item_t* pcmd = NULL; /* pointer to tracked command item */
          /* obtain a free command item */
          ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
          if ( pcmd != NULL )
          {
            pcmd->type = CSD_VS_CMD_SEND_DTMF_DETECTED;
            pcmd->cmd.dtmf_detected = APRV2_PKT_GET_PAYLOAD( csd_vs_event_rx_dtmf_detected_t, packet);
            pcmd->client_token = packet->token;
			pcmd->psess = psess;
            ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
            /* signal the csd common thread to process the cmd */
            sigs |= CSD_VOC_COMMON_SIG_CMD;
            ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
          }
          CSD_MSG(CSD_PRIO_LOW,
                        "csd_vs_apr_callback: DTMF detected request" );

        }
        break;


        case VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST:
        {
          csd_vs_cmd_list_item_t* pcmd = NULL;

          ( void ) apr_objmgr_find_object ( &csd_vs_apr_objmgr, packet->dst_port, &apr_obj );

          if ( apr_obj != NULL )
          {
            psess = apr_obj->any.ptr;
          }

          /* obtain a free command item */
          ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
          if ( pcmd != NULL )
          {
            pcmd->type = CSD_VS_CMD_SEND_DEC_BUF_REQ_CALLBACK;
            pcmd->psess = psess;
            ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
            /* signal the csd common thread to process the cmd */
            sigs |= CSD_VOC_COMMON_SIG_CMD;
            ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
          }
          CSD_MSG(CSD_PRIO_LOW,
                        "csd_vs_apr_callback: VOIP DEC Buffer Request" );
        }
        break;

        case VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY:
        {
          csd_vs_cmd_list_item_t* pcmd = NULL;

          CSD_MSG(CSD_PRIO_LOW,
                      "csd_vs_apr_callback: VOIP ENC Buffer Ready" );

          ( void ) apr_objmgr_find_object ( &csd_vs_apr_objmgr, packet->dst_port, &apr_obj );

          if ( apr_obj != NULL )
          {
            psess = apr_obj->any.ptr;
          }

          if ( NULL != psess )
          {
            /* notify middleware of the new incoming packet */
            ( void ) apr_list_remove_head( &csd_vs_free_cmd_list, ( ( apr_list_node_t**) &pcmd ) );
            if ( pcmd != NULL)
            {
              pcmd->type = CSD_VS_CMD_SEND_ENC_BUF_READY_CALLBACK;
              pcmd->psess = psess;
              ( void ) apr_list_add_tail( &csd_vs_active_cmd_list, &pcmd->link );
              /* signal the csd common thread to process the cmd */
              sigs |= CSD_VOC_COMMON_SIG_CMD;
              ( void ) csd_voc_common_queue_cmd( h_csd_voc_common, sigs);
            }
          }
          break;
        }

        case APRV2_IBASIC_EVT_ACCEPTED:
        break;

        case VSS_IMEMORY_RSP_MAP:  /* Calibration Memory map success case */
        {
          vss_imemory_rsp_map_t* pmemory_map_rsp = NULL;

          pmemory_map_rsp = APRV2_PKT_GET_PAYLOAD( vss_imemory_rsp_map_t, packet);

          if ( NULL == csd_vs_mapped_pmem_handle && !psess->b_voip_mem_map)
          {
            csd_vs_mapped_pmem_handle = pmemory_map_rsp->mem_handle;
          }
          else if ( psess->b_voip_mem_map )
          {
            psess->voip_pkt_cfg.mapped_pmem_handle = pmemory_map_rsp->mem_handle;
            psess->b_voip_mem_map = FALSE;
          }

          /* Signal thread in csd_vs_open() out of wait state */
          csd_event_signal ( psess->csd_vs_event );
        }
        break;

        default:
        {
          CSD_MSG( CSD_PRIO_LOW, "csd_vs_apr_callback: unsupported packet opcode");
        }
        break;
        } /* switch( packet->opcode ) */
      } /*  if( NULL != psess ) */
    } /* if ( apr_obj != NULL ) */
  } /* if ( NULL != packet ) */

  if ( NULL != packet )
  {
    __aprv2_cmd_free( csd_vs_apr_handle, packet );
  }

  /* Note that rc needs to be APR_EOK in order to prevent APR from freeing the packet a second time */
  return APR_EOK;
}

/* uint32_t () {*/
CSD_INTERNAL int32_t csd_vs_init ( struct csd_vtable** vtable )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint8_t i = 0;
  uint8_t checkpoint = 0;

  {
    rc = csd_voc_common_get_session( csd_vs_command_fn,
                                     &h_csd_voc_common );
  }

  if ( ( CSD_EOK == rc )
       && ( vtable ) )
  {
    checkpoint = 1;
    /* CSD vs get session success */
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_VS_INIT_STAGE,
                       //CSD_INIT_VS_SESSION_GET,0,0); 
    csd_vs_vtable.open_fn = ( csd_open_fn ) csd_vs_open;
    csd_vs_vtable.close_fn = ( csd_close_fn )csd_vs_close;
    csd_vs_vtable.ioctl_fn = ( csd_ioctl_fn )csd_vs_ioctl;
    csd_vs_vtable.read_fn = ( csd_read_fn )csd_vs_read;
    csd_vs_vtable.write_fn = ( csd_write_fn ) csd_vs_write;
    *vtable = &csd_vs_vtable;
  }
  else
  {
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_VS_INIT_STAGE,
                     //  CSD_INIT_VS_SESSION_GET,0,0); 
  }
  if ( CSD_EOK == rc )
  {
    apr_rc = __aprv2_cmd_local_dns_lookup( (char_t* )csd_vs_cvs_dest_dns,
                                       sizeof( csd_vs_cvs_dest_dns),
                                       &csd_vs_cvs_dest_addr);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                       //  CSD_INIT_VS_DNS_CVS_LOOK_UP,0,0); 
  }
  if ( APR_EOK == apr_rc )
  {
    apr_rc = __aprv2_cmd_local_dns_lookup( (char_t* )csd_vs_mvm_dest_dns,
                                       sizeof( csd_vs_mvm_dest_dns),
                                       &csd_vs_mvm_dest_addr);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                       //  CSD_INIT_VS_DNS_MVM_LOOK_UP,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    apr_rc = __aprv2_cmd_register2( &csd_vs_apr_handle,
                                csd_vs_dns,
                                sizeof ( csd_vs_dns ),
                                0,
                                csd_vs_apr_callback,
                                NULL,
                                &csd_vs_src_addr );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                        // CSD_INIT_VS_APR_REGISTER,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 2;
    apr_rc = csd_lock_create( &csd_vs_task_lock );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_APR_LOCK_CREATE,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 3;
    apr_rc = apr_list_init( &csd_vs_free_cmd_list, csd_vs_task_lock_fn, csd_vs_task_unlock_fn );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_FREE_CMD_LIST_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 4;
    apr_rc = apr_list_init( &csd_vs_active_cmd_list, csd_vs_task_lock_fn, csd_vs_task_unlock_fn );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_ACTIVE_CMD_LIST_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 5;
    apr_rc = apr_list_init( &csd_vs_track_dispatched_cmd_list, csd_vs_task_lock_fn, csd_vs_task_unlock_fn );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_DISPATCH_CMD_LIST_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 6;
    for ( i = 0; i < CSD_VS_NUM_COMMANDS; ++i )
    {
      if ( APR_EOK != ( apr_rc = apr_list_add_tail( &csd_vs_free_cmd_list,
                                              ( apr_list_node_t* ) &csd_vs_cmd_pool[i] ) ) )
      {
        break;
      }
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_FREE_CMD_LIST_ADD,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    apr_rc = apr_list_init( &csd_vs_free_sess_list, csd_vs_task_lock_fn, csd_vs_task_unlock_fn );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                       //  CSD_INIT_VS_FREE_SESS_LIST_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 7;
    apr_rc = apr_list_init( &csd_vs_active_sess_list, csd_vs_task_lock_fn, csd_vs_task_unlock_fn );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                        // CSD_INIT_VS_ACTIVE_SESS_LIST_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    checkpoint = 8;
    for ( i = 0; i < CSD_VS_NUM_SESSIONS; ++i )
    {
      if ( APR_EOK != ( apr_rc = apr_list_add_tail( &csd_vs_free_sess_list,
                            ( ( apr_list_node_t* ) &(csd_vs_sess_pool[i].link) ) ) ) )
      {
        break;
      }
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                        // CSD_INIT_VS_FREE_SESS_LIST_ADD,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
    params.table = csd_vs_object_table;
    params.total_bits = CSD_VS_HANDLE_TOTAL_BITS_V;
    params.index_bits = CSD_VS_HANDLE_INDEX_BITS_V;
    params.lock_fn = csd_vs_task_lock_fn;
    params.unlock_fn = csd_vs_task_unlock_fn;
    apr_rc = apr_objmgr_construct( &csd_vs_apr_objmgr, &params );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_OBJECT_MGR_INIT,0,0); 
  }

  if ( APR_EOK != apr_rc )
  {
    /* free resources here*/
    CSD_MSG( CSD_PRIO_ERROR,
                 "csd_vs_init: initialization error, checkpoint[0x%x]",
                checkpoint );
    switch ( checkpoint )
    {
    case 8:
      ( void ) apr_list_destroy ( &csd_vs_active_sess_list );
      /*csd vs free command list done*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_ACTIVE_SESS_LIST_INIT,0,0); 
    case 7:
      ( void ) apr_list_destroy ( &csd_vs_free_sess_list );
      /*csd vs free command list done*/

      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_FREE_SESS_LIST_INIT,0,0); 
    case 6:
      ( void ) apr_list_destroy ( &csd_vs_track_dispatched_cmd_list );
      /*csd vs free command list done*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_DISPATCH_CMD_LIST_INIT,0,0); 
    case 5:
      ( void ) apr_list_destroy ( &csd_vs_free_cmd_list );
      /*csd vs free command list done*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_FREE_CMD_LIST_INIT,0,0); 
    case 4:
      ( void ) apr_list_destroy ( &csd_vs_active_cmd_list );
      /*csd vs active command list done*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_ACTIVE_CMD_LIST_INIT,0,0); 
    case 3:
      ( void ) csd_lock_destroy ( csd_vs_task_lock );
      /*csd vs lock destroy done*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_APR_LOCK_CREATE,0,0); 
    case 2:
      ( void ) __aprv2_cmd_deregister( csd_vs_apr_handle);
      /*csd vs de register  apr done*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_APR_REGISTER,0,0); 
    case 1:
      ( void ) csd_voc_common_free_session ( &h_csd_voc_common );
      /*csd vs free session*/
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                         //CSD_INIT_VS_SESSION_GET,0,0); 
      break;
    default:
      break;
    }/* switch */
    rc = csd_map_apr_error_to_csd ( apr_rc );
  }
  return rc;
}

CSD_INTERNAL int32_t csd_vs_deinit ( void )
{

  ( void ) apr_list_destroy ( &csd_vs_active_sess_list );
  /*csd vs active session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_ACTIVE_SESS_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vs_free_sess_list );
  /*csd vs free session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_FREE_SESS_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vs_track_dispatched_cmd_list );
  /*csd vs dispatch command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_DISPATCH_CMD_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vs_free_cmd_list );
  /*csd vs free command list destroydone*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_FREE_CMD_LIST_INIT,0,0); 

  ( void ) apr_list_destroy ( &csd_vs_active_cmd_list );
  /*csd vs active command list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_ACTIVE_CMD_LIST_INIT,0,0); 

  ( void ) csd_lock_destroy ( csd_vs_task_lock );
  /*csd vs lock destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_APR_LOCK_CREATE,0,0); 


  ( void ) __aprv2_cmd_deregister( csd_vs_apr_handle);
  /*csd vs de register  apr done*/
   //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                      //CSD_INIT_VS_APR_REGISTER,0,0); 


  { /* Release the object management. */
    ( void ) apr_objmgr_destruct( &csd_vs_apr_objmgr );
    /*csd vs object manager done*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                       //CSD_INIT_VS_OBJECT_MGR_INIT,0,0); 
  }
  ( void ) csd_voc_common_free_session ( &h_csd_voc_common );
  /*csd vs free session*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VS_INIT_STAGE,
                     //CSD_INIT_VS_SESSION_GET,0,0); 

  return CSD_EOK;
}

/* this call needs to be syncronous */
CSD_INTERNAL int32_t csd_vs_open (
  uint32_t open_id,
  void* params,
  uint32_t size )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vs_session_item_t* psess = NULL;
  apr_objmgr_object_t* objmgr_obj = NULL;
  csd_vs_open_t* csd_vs_cmd = ( csd_vs_open_t* )params;
  uint32_t nContigLen  = 0;

  /*validate session is unique*/
  if( csd_vs_session_exsists(csd_vs_cmd->u.full_control.name) )
  {
    rc = CSD_EALREADY;
    CSD_MSG( CSD_PRIO_ERROR, "csd_vs_open: Failed!! Session with same name already exsists");
  }
  else
  {

    /* Get a session */
    apr_rc = apr_list_remove_head( &csd_vs_free_sess_list, ( ( apr_list_node_t** )&psess) );

    if ( ( APR_EOK == apr_rc ) &&
         ( NULL != psess ) )
    {
      memset( ( void* )psess, 0, sizeof( csd_vs_session_item_t ) );

      ( void ) apr_list_add_tail( &csd_vs_active_sess_list, &psess->link );

      ( void ) apr_objmgr_alloc_object ( &csd_vs_apr_objmgr, &objmgr_obj );

      if ( NULL != objmgr_obj )
      {
        objmgr_obj->any.ptr = psess;
        psess->h_apr_obj = objmgr_obj->handle;
      }
    }

    if ( APR_EOK == apr_rc )
    {
      apr_rc = csd_event_create ( &psess->csd_vs_event );
    }

    rc = csd_map_apr_error_to_csd( apr_rc );

    if( ( csd_vs_active_sess_list.size == 1 ) &&
        ( NULL == csd_vs_va_cal ) &&
        ( NULL != psess ) )
    {
      /* Initialize PMEM Buf for Voice Stream Calibration */
      uint32_t cal_read = 0;
      vss_imemory_cmd_map_physical_t vss_map_physical;

      memset ( ( void* )&vss_map_physical, 0, sizeof ( vss_imemory_cmd_map_physical_t ) );

      csd_vs_cal_buf_size = csd_voc_common_get_calibration_size ( CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_SIZE,
                                      NULL );

      if( csd_vs_cal_buf_size > 0 )
      {
        csd_vs_cal_buf_size = ( csd_vs_cal_buf_size / CSD_MEM_PAGE_SIZE ) * CSD_MEM_PAGE_SIZE + CSD_MEM_PAGE_SIZE;

        rc = csd_mem_alloc( csd_vs_cal_buf_size, &buffer_memory_handle, &csd_vs_va_cal );

        /* obtain the physical address of common calibration */
        if ( csd_vs_va_cal != NULL )
        {
          if ( 0 != csd_get_mem_info( buffer_memory_handle,
                                    &csd_vs_pa_cal,
                                    &nContigLen ) )
          {
            rc = CSD_EFAILED;
            CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: PMEM physical mapping error csd_vs_va_cal[0x%p]", ((void*)csd_vs_va_cal) );
          }
        }
        else
        {
          rc = CSD_ENORESOURCE;
          CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: PMEM allocation for STREAM cal" );
        }

        /* Read Common Calibration */
        if ( rc == CSD_EOK )
        {
          rc = csd_voc_common_get_calibration ( CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC,
                                                NULL,
                                                ( uint8_t *)csd_vs_va_cal,
                                                csd_vs_cal_buf_size,
                                                &cal_read );
          csd_vs_cal_size = cal_read;

          if ( ( cal_read <= 0 ) || ( CSD_EOK != rc ) )
          {
            CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: OEMDB STREAM cal read error" );
          }
        }

    /* Memory Table Buffer Layout constructed according to specs
    by vss_imemory_public_if.h with the virtual address append
    at the end.

    The memory table may require more than 4K to
    describe non-contiguous VC calibration buffer.

   The table descriptor only holds the physical address of the
   next memory table. CSD will reserve the last 12 bytes of the
   memory table to populate the virtual address of the next
   memory table to avoid translating the address during free.

    <------------------- 12 bytes ------------------------->
     ------------------------------------------------------
    | vss_imemory_table_descriptor_t next_table_descriptor |
     ------------------------------------------------------
    | vss_imemory_block_t block_0                          |
     ------------------------------------------------------
    | vss_imemory_block_t block_1                          |
     ------------------------------------------------------
    | ...                                                  |
     ------------------------------------------------------
    | vss_imemory_block_t block_339                        |
     ------------------------------------------------------
    | vss_imemory_table_t* pVANextMemoryTable                          |
     ------------------------------------------------------
    <----------8 bytes----------> ( Assuming 64 bit addressing )

  */

        if ( ( cal_read > 0 ) &&
           ( CSD_EOK == rc ) )
        {
           /* Allocate the Memory Table for Non-contiguous memory */
          uint32_t nTraverseSize = 0;
          uint8_t* pTraverse = NULL;
          uint64_t pPhysical = NULL;
          uint8_t* pVAMemoryTableTraverse = NULL;
          vss_imemory_table_t* vss_mem_tbl = NULL;
          vss_imemory_block_t* vss_mem_block = NULL;
    #if 0
          /* if there are over the maximum number of memory blocks per 4k Page Size */
          uint8_t* pNewVAMemoryTable = NULL;
    #endif

          /* allocate first memory table here */
          rc = csd_mem_alloc( CSD_MEM_PAGE_SIZE, &table_memory_handle, &csd_vs_pVAMemoryTable );

          if( NULL != csd_vs_pVAMemoryTable)
          {
             memset( ( void* )csd_vs_pVAMemoryTable, 0, CSD_MEM_PAGE_SIZE );
          }
          else
          {
            CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: csd_mem_alloc failed\n" );
            rc = CSD_ENORESOURCE;
          }
          if ( CSD_EOK == rc )
          {
            pVAMemoryTableTraverse = csd_vs_pVAMemoryTable;
            vss_mem_tbl = ( vss_imemory_table_t* )pVAMemoryTableTraverse;

            /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
            pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_table_descriptor_t );

            /* set the next table description to 0 at the moment */
            vss_mem_tbl->next_table_descriptor.mem_address = 0;
            vss_mem_tbl->next_table_descriptor.mem_size = 0;

            /* determine the amount of memory blocks there are in the non-contiguous memory */
            nTraverseSize = csd_vs_cal_buf_size;
            pTraverse = csd_vs_va_cal;

            while ( nTraverseSize > 0 )/* exit the loop when the entire buffer has been analyzed */
            {
              csd_get_mem_info( buffer_memory_handle, &pPhysical, &nContigLen );

              nTraverseSize = nTraverseSize - nContigLen; /* update the left over size to be traversed */
              pTraverse = pTraverse + nContigLen; /* update pointer to the virual address of the next memory block */

              vss_mem_block = ( vss_imemory_block_t* )pVAMemoryTableTraverse;

              vss_mem_block->mem_address = pPhysical;
              vss_mem_block->mem_size = nContigLen; /* this needs to be updated cannot send the entire buffer down to dsp */

              CSD_MSG(CSD_PRIO_LOW, "csd_vs_open: PMEM Cal Data Block[#%d], size[%d], address[0x%016llX]",
                     csd_vs_memory_blocks, vss_mem_block->mem_size, vss_mem_block->mem_address );

              pVAMemoryTableTraverse = (uint8_t*)(vss_mem_block++);

              csd_vs_memory_blocks++;
              /* if there are over the maximum number of memory blocks per 4k Page Size */
  #if 0
              if ( csd_vs_memory_blocks >=
                 ( ( ( CSD_PMEM_PAGE_SIZE - sizeof ( vss_imemory_table_descriptor_t ) - sizeof ( vss_imemory_table_t* ) ) / sizeof ( vss_imemory_block_t ) ) ) )
              {
                /* allocate the new memory table */
                pNewVAMemoryTable = ( uint8_t * ) CSD_PMEM_ALLOC_EXT( CSD_PMEM_PAGE_SIZE, PMEM_TYPE_ID, PMEM_NON_CONTIG, PMEM_ALIGNMENT_4K );
                 /* copy the VA of the next memory table at the end of this current one */
                ( void ) memcpy ( pVAMemoryTableTraverse, &pNewVAMemoryTable, sizeof ( vss_imemory_table_t * ) );

                /* get the Physical Address for the Memory Table */
                CSD_MEM_OFFSET( pNewVAMemoryTable, CSD_PMEM_PAGE_SIZE,
                              &pPhysical, &nContigLen);

                vss_mem_tbl->next_table_descriptor.mem_address = pPhysical;

                vss_mem_tbl->next_table_descriptor.mem_size = nContigLen;

                pVAMemoryTableTraverse = pNewVAMemoryTable;
                vss_mem_tbl = pVAMemoryTableTraverse;

                /* set the next table description to 0 at the moment */
                vss_mem_tbl->next_table_descriptor.mem_address = 0;
                vss_mem_tbl->next_table_descriptor.mem_size = 0;

                /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
                pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_table_descriptor_t );
              }
  #endif
              /*IMPORTANT: what about where to free this */
            }/* while ( nTraverseSize > 0 ) */
            /* populate the rest of the mapping parameters */

            /* get the Physical Address for the Memory Table */
            csd_get_mem_info( table_memory_handle, &pPhysical, &nContigLen );

            vss_map_physical.table_descriptor.mem_address = pPhysical;
            CSD_MSG(CSD_PRIO_LOW, "csd_vs_open: PA of the current table descriptor[0x%016llX])", vss_map_physical.table_descriptor.mem_address );


            /* Register PMEM Buf for Voice Stream Calibration */
            vss_map_physical.table_descriptor.mem_size = csd_vs_memory_blocks * sizeof ( vss_imemory_block_t ) + sizeof ( vss_imemory_table_descriptor_t );

            vss_map_physical.is_cached = TRUE;
            vss_map_physical.cache_line_size = CSD_VOC_COMMON_CACHE_LINE_SIZE;
            vss_map_physical.access_mask = vss_map_physical.access_mask | 1;
            vss_map_physical.page_align = CSD_VOC_COMMON_PAGE_ALIGN;
            vss_map_physical.min_data_width = CSD_VOC_COMMON_MIN_DATA_WIDTH;
            vss_map_physical.max_data_width = CSD_VOC_COMMON_MAX_DATA_WIDTH;

            apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                        ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                        csd_vs_src_addr,
                                        ( uint16_t )psess->h_apr_obj,
                                        csd_vs_mvm_dest_addr,
                                        APR_NULL_V,
                                        0,
                                        ( uint32_t )VSS_IMEMORY_CMD_MAP_PHYSICAL,
                                        (void*)&vss_map_physical,
                                        ( uint32_t )sizeof( vss_imemory_cmd_map_physical_t )
                                        );
            CSD_MSG( CSD_PRIO_HIGH,
                    "csd_vs_open: VSS_IMEMORY_CMD_MAP_PHYSICAL sent, apr_rc[0x%x]",
                    apr_rc);
            /* Wait for Mapping Command's response */
            if ( APR_EOK == apr_rc )
            {
              apr_rc = csd_event_wait ( psess->csd_vs_event );
            }

            if ( APR_EOK != apr_rc )
            {
              CSD_MSG( CSD_PRIO_ERROR,
                    "csd_vs_open: csd_event_wait failed when waiting for Map Response, apr_rc[0x%x]",
                    apr_rc);
            }
            rc = csd_map_apr_error_to_csd ( apr_rc );
          }
        }
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_vs_open: Unable to find cal data in ACDB, skipping" );
      }
    }

    if( ( CSD_EOK == rc ) && ( NULL != psess ) )
    {
      switch ( csd_vs_cmd->open_id )
      {
      case CSD_VS_OPENID_FULL_CONTROL:
        {
          vss_istream_cmd_create_full_control_session_t cvs_cmd;
          aprv2_packet_t* papr_packet = NULL;

          cvs_cmd.dec_media_type = csd_vs_cmd->u.full_control.dec_media_type;
          cvs_cmd.enc_media_type = csd_vs_cmd->u.full_control.enc_media_type;
          cvs_cmd.direction = csd_vs_cmd->u.full_control.direction;
          cvs_cmd.network_id = csd_vs_cmd->u.full_control.network_id;

          psess->dec_type = csd_vs_cmd->u.full_control.dec_media_type;
          psess->enc_type = csd_vs_cmd->u.full_control.enc_media_type;

          psess->voip_pkt_cfg.pVABuf = NULL;
          psess->voip_pkt_cfg.pVABufEnc = NULL;
          psess->voip_pkt_cfg.pVABufDec = NULL;
          psess->voip_pkt_cfg.pVAMemoryTable = NULL;
          psess->voip_pkt_cfg.mapped_pmem_handle = 0;
          psess->voip_pkt_cfg.buf_size = CSD_MEM_PAGE_SIZE;
          psess->voip_pkt_cfg.pa_buf = 0;
          psess->voip_pkt_cfg.pBufHandle = NULL;
          psess->voip_pkt_cfg.pTableMemHandle = NULL;
          psess->b_voip_mem_map = FALSE;

          /*  Stream packet size is now the max paylod size of the OoB shared buffer  */
          psess->stream_pkt_size = ( CSD_MEM_PAGE_SIZE / 2 ) - sizeof ( csd_vs_voip_buffer_t );

          apr_rc = __aprv2_cmd_alloc_ext ( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     psess->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )NULL,
                                     ( uint32_t )NULL,
                                     ( uint32_t )VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION,
                                     sizeof ( vss_istream_cmd_create_full_control_session_t ) + csd_vs_cmd->u.full_control.name_size,
                                     &papr_packet );

          if( ( APR_EOK == apr_rc ) && ( papr_packet != NULL ) )
          { /* copy the param_data into apr buffer */
            uint8_t* pdata;
            uint32_t psize;
            pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
            psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(papr_packet->header);
            csd_memscpy( pdata, psize,
                         &cvs_cmd, sizeof ( vss_istream_cmd_create_full_control_session_t ) ); /* copy the cmd structure */
            pdata += sizeof( vss_istream_cmd_create_full_control_session_t );
            psize -= sizeof( vss_istream_cmd_create_full_control_session_t );
            csd_memscpy( pdata, psize,
                         csd_vs_cmd->u.full_control.name, csd_vs_cmd->u.full_control.name_size );

            apr_rc = __aprv2_cmd_forward( csd_vs_apr_handle, papr_packet );
            CSD_MSG( CSD_PRIO_HIGH,
                   "csd_vs_open: VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION sent, apr_rc[0x%x]",
                   apr_rc);
          }

          if ( APR_EOK == apr_rc )
          {
            apr_rc = csd_event_wait ( psess->csd_vs_event  );
          }

          rc = csd_map_apr_error_to_csd ( apr_rc );
          /* Allocate and map buffer for VOIP */
          if ( APR_EOK == apr_rc )
          {
            rc = csd_mem_alloc( psess->voip_pkt_cfg.buf_size, &psess->voip_pkt_cfg.pBufHandle, &psess->voip_pkt_cfg.pVABuf );
          }

          /* obtain the physical address of voip packet buffer */
          if ( psess->voip_pkt_cfg.pVABuf != NULL )
          {
            if ( 0 != csd_get_mem_info( psess->voip_pkt_cfg.pBufHandle, &psess->voip_pkt_cfg.pa_buf,&nContigLen ) )
            {
              CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: PMEM physical mapping error psess->voip_pkt_cfg.pVABuf=[0x%p])", ((void*)psess->voip_pkt_cfg.pVABuf) );
            }
            else
            {
              /* Allocate the Memory Table for Non-contiguous memory for Shared VOIP Buffer */
              vss_imemory_cmd_map_physical_t vss_map_physical;
              uint64_t pPhysical = NULL;
              uint8_t* pVAMemoryTableTraverse = NULL;
              vss_imemory_table_t* vss_mem_tbl = NULL;
              vss_imemory_block_t* vss_mem_block = NULL;

              memset ( ( void* )&vss_map_physical, 0, sizeof ( vss_imemory_cmd_map_physical_t ) );

              /* allocate memory table here */
              rc = csd_mem_alloc( CSD_MEM_PAGE_SIZE, &psess->voip_pkt_cfg.pTableMemHandle, &psess->voip_pkt_cfg.pVAMemoryTable );

              if ( NULL != psess->voip_pkt_cfg.pVAMemoryTable )
              {
                memset( ( void* )psess->voip_pkt_cfg.pVAMemoryTable, 0, CSD_MEM_PAGE_SIZE );
              }
              else
              {
                CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: csd_mem_alloc failed\n" );
                rc = CSD_ENORESOURCE;
              }
              if (CSD_EOK == rc)
              {
                pVAMemoryTableTraverse = psess->voip_pkt_cfg.pVAMemoryTable;
                vss_mem_tbl = ( vss_imemory_table_t* )pVAMemoryTableTraverse;

                /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
                pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_table_descriptor_t );

                /* set the next table description to 0 at the moment */
                vss_mem_tbl->next_table_descriptor.mem_address = 0;
                vss_mem_tbl->next_table_descriptor.mem_size = 0;

                /* Find physical address of shared VOIP buffer to map it to vss mem block */
                csd_get_mem_info( psess->voip_pkt_cfg.pBufHandle, &pPhysical, &nContigLen );

                vss_mem_block = ( vss_imemory_block_t* )pVAMemoryTableTraverse;
                vss_mem_block->mem_address = pPhysical;
                vss_mem_block->mem_size = CSD_MEM_PAGE_SIZE; /* this needs to be updated cannot send the entire buffer down to dsp */

                CSD_MSG(CSD_PRIO_LOW, "csd_vs_open: PMEM OoB Shared Buffer Block size[%d], address[0x%016llX])", vss_mem_block->mem_size, vss_mem_block->mem_address );

                /* get the Physical Address for the Memory Table to be used in VSS Map Physical struct */
                csd_get_mem_info( psess->voip_pkt_cfg.pTableMemHandle, &pPhysical, &nContigLen );

                vss_map_physical.table_descriptor.mem_address = pPhysical;
                CSD_MSG(CSD_PRIO_LOW, "csd_vs_open: PA of the current table descriptor[0x%016llX])", vss_map_physical.table_descriptor.mem_address );


                /* Map the VOIP Shared Buffer */
                /* Size of VOIP Buffer is One imemory block = 4KB */
                vss_map_physical.table_descriptor.mem_size = sizeof ( vss_imemory_block_t ) + sizeof ( vss_imemory_table_descriptor_t );

                vss_map_physical.is_cached = TRUE;
                vss_map_physical.cache_line_size = CSD_VOC_COMMON_CACHE_LINE_SIZE;
                vss_map_physical.access_mask = vss_map_physical.access_mask | 1;
                vss_map_physical.page_align = CSD_VOC_COMMON_PAGE_ALIGN;
                vss_map_physical.min_data_width = CSD_VOC_COMMON_MIN_DATA_WIDTH;
                vss_map_physical.max_data_width = CSD_VOC_COMMON_MAX_DATA_WIDTH;
                psess->b_voip_mem_map = TRUE;

                apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                            ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                            csd_vs_src_addr,
                                            ( uint16_t )psess->h_apr_obj,
                                            csd_vs_mvm_dest_addr,
                                            APR_NULL_V,
                                            0,
                                            ( uint32_t )VSS_IMEMORY_CMD_MAP_PHYSICAL,
                                            (void*)&vss_map_physical,
                                            ( uint32_t )sizeof( vss_imemory_cmd_map_physical_t )
                                            );

                CSD_MSG( CSD_PRIO_HIGH,
                     "csd_vs_open: VSS_IMEMORY_CMD_MAP_PHYSICAL sent, apr_rc[0x%x]",
                     apr_rc);

                /* Wait for Mapping Command's response */
                if ( APR_EOK == apr_rc )
                {
                  apr_rc = csd_event_wait ( psess->csd_vs_event  );
                }

                if ( APR_EOK == apr_rc && psess->voip_pkt_cfg.mapped_pmem_handle != NULL )
                {
                  vss_istream_cmd_set_packet_exchange_mode_t vss_pkt_exchange_mode;

                  vss_pkt_exchange_mode.mode = VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND;

                  apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                              ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                              csd_vs_src_addr,
                                              ( uint16_t )psess->h_apr_obj,
                                              csd_vs_cvs_dest_addr,
                                              ( uint16_t )psess->h_cvs,
                                              0,
                                              ( uint32_t )VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE,
                                              (void*)&vss_pkt_exchange_mode,
                                              ( uint32_t )sizeof( vss_istream_cmd_set_packet_exchange_mode_t )
                                              );

                  CSD_MSG( CSD_PRIO_HIGH,
                           "csd_vs_open: VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE sent, apr_rc[0x%x]",
                            apr_rc);
                }

                /* Wait for Set Packet Exchange Mode Response */
                if ( APR_EOK == apr_rc )
                {
                  apr_rc = csd_event_wait ( psess->csd_vs_event  );
                }

                if ( APR_EOK == apr_rc )
                {
                  /* After setting Packet Exchange to OoB, set up OoB Configuration  */
                  vss_istream_cmd_set_oob_packet_exchange_config_t vss_oob_pkt_config;

                  /* Obtain virtual and physical addresses of enc and dec buffers */
                  psess->voip_pkt_cfg.pVABufEnc = psess->voip_pkt_cfg.pVABuf;
                  psess->voip_pkt_cfg.pVABufDec = psess->voip_pkt_cfg.pVABuf + ( psess->voip_pkt_cfg.buf_size / 2 );

                  vss_oob_pkt_config.mem_handle = psess->voip_pkt_cfg.mapped_pmem_handle;
                  vss_oob_pkt_config.enc_buf_addr = (uint64_t) psess->voip_pkt_cfg.pa_buf;
                  vss_oob_pkt_config.enc_buf_size = (uint32_t) psess->voip_pkt_cfg.buf_size / 2;
                  vss_oob_pkt_config.dec_buf_addr = (uint64_t) ( psess->voip_pkt_cfg.pa_buf + ( psess->voip_pkt_cfg.buf_size / 2 ) );
                  vss_oob_pkt_config.dec_buf_size = (uint32_t) psess->voip_pkt_cfg.buf_size / 2;

                  /* After setting packet exchange mode to out of band, send VOIP config data */
                  apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                              ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                              csd_vs_src_addr,
                                              ( uint16_t )psess->h_apr_obj,
                                              csd_vs_cvs_dest_addr,
                                              ( uint16_t )psess->h_cvs,
                                              0,
                                              ( uint32_t )VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG,
                                              (void*)&vss_oob_pkt_config,
                                              ( uint32_t )sizeof( vss_istream_cmd_set_oob_packet_exchange_config_t )
                                              );
                  CSD_MSG( CSD_PRIO_HIGH,
                          "csd_vs_open: VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG sent, apr_rc[0x%x] "
                          "VoIP OoB Config: mem_handle[0x%x], enc_buf_addr[0x%016llX], enc_buf_size[%d], dec_buf_addr[0x%016llX], dec_buf_size[%d]",
                          apr_rc,vss_oob_pkt_config.mem_handle, vss_oob_pkt_config.enc_buf_addr, vss_oob_pkt_config.enc_buf_size,
                          vss_oob_pkt_config.dec_buf_addr, vss_oob_pkt_config.dec_buf_size);
                }

                /* Wait for Set Packet Exchange Configuration Response */
                if ( APR_EOK == apr_rc )
                {
                  apr_rc = csd_event_wait ( psess->csd_vs_event  );
                }

                if ( APR_EOK != apr_rc )
                {
                  CSD_MSG( CSD_PRIO_ERROR,
                       "csd_vs_open: csd_event_wait failed when waiting for Set OoB Set Packet Exchange Config Response, apr_rc[0x%x]",
                       apr_rc);
                }
                rc = csd_map_apr_error_to_csd ( apr_rc );
              }
            }
          }
          else
          {
            CSD_MSG(CSD_PRIO_ERROR, "csd_vs_open: VoIP PMEM shared buffer Alloc Error");
          }

          if ( ( NULL != psess->h_cvs ) )
          {
            if ( NULL != objmgr_obj )
            {
              psess->h_csd = csd_handle_malloc( CSD_OPEN_VOICE_STREAM, ( void* )&objmgr_obj->handle );
            }

            psess->csd_vs_cb = ( csd_vs_event_callback_fn_t )csd_vs_cmd->u.full_control.callback_fn;
            psess->csd_vs_cb_data = csd_vs_cmd->u.full_control.cb_data;

            strlcpy ( psess->vs_name,
            csd_vs_cmd->u.full_control.name,
            sizeof( psess->vs_name ) );

            psess->vs_name_size = csd_vs_cmd->u.full_control.name_size;

            /*update state*/
            csd_vs_update_state (psess, VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION);
          }
          else
          {
            rc = CSD_EFAILED;
          }
        }
      break;

      case CSD_VS_OPENID_PASSIVE_CONTROL:
        {
          apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     psess->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )NULL,
                                     ( uint32_t )NULL,
                                     ( uint32_t )VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION,
                                     (void*) csd_vs_cmd->u.passive_control.name,
                                     ( uint32_t )csd_vs_cmd->u.passive_control.name_size
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                   "csd_vs_open: VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION sent, apr_rc[0x%x]",
                   apr_rc);

          if ( APR_EOK == apr_rc )
          {
            apr_rc = csd_event_wait ( psess->csd_vs_event );
          }

          if ( APR_EOK != apr_rc )
          {
            CSD_MSG( CSD_PRIO_ERROR,
                  "csd_vs_open: csd_event_wait failed when waiting for Passive Control Session response, apr_rc[0x%x]",
                  apr_rc);
          }
          rc = csd_map_apr_error_to_csd ( apr_rc );

          if ( NULL != psess->h_cvs )
          {
            if ( NULL != objmgr_obj )
            {
              psess->h_csd = csd_handle_malloc( CSD_OPEN_VOICE_STREAM, ( void* )&objmgr_obj->handle );
            }

            psess->csd_vs_cb = ( csd_vs_event_callback_fn_t )csd_vs_cmd->u.passive_control.callback_fn;
            psess->csd_vs_cb_data = csd_vs_cmd->u.passive_control.cb_data;

            strlcpy( psess->vs_name,
                     csd_vs_cmd->u.passive_control.name,
                     sizeof( psess->vs_name ) );

            psess->vs_name_size = csd_vs_cmd->u.passive_control.name_size;

             /*update state*/
            csd_vs_update_state (psess, VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION);

          }
          else
          {
            rc = CSD_EFAILED;
          }
        }
        break;

      default:
          rc = CSD_EUNSUPPORTED;
        break;
      }/* switch statement */
    }/* if ( rc == APR_EOK*/

    if ( ( CSD_EOK != rc ) &&
         ( NULL != psess ) )
    { /* free resources if mvs did not return a valid handle */
      if ( NULL != psess->h_csd )
      {
        ( void )csd_handle_free ( psess->h_csd );
      }

      if ( ( NULL != objmgr_obj ) &&
         ( NULL != objmgr_obj->handle ))
      {
        ( void ) apr_objmgr_free_object ( &csd_vs_apr_objmgr, objmgr_obj->handle );
      }

      if ( NULL != psess->csd_vs_event )
      {
        ( void ) csd_event_destroy ( psess->csd_vs_event );
      }

      ( void ) apr_list_delete ( &csd_vs_active_sess_list, &( psess->link ) );
      ( void ) apr_list_add_tail( &csd_vs_free_sess_list, &( psess->link ) );
    }
  }
  *csd_vs_cmd->ret_status = rc;

  if ( ( CSD_EOK == rc ) && ( NULL != psess ) )
  {
    return psess->h_csd;
  }
  else
  {
    return 0;/* return a 0 handle if there is an error */
  } 
}

CSD_INTERNAL int32_t csd_vs_close ( uint32_t handle )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  vss_imemory_cmd_unmap_t cvs_cmd;
  vss_imemory_table_t* pMemoryTable = NULL;
  vss_imemory_table_t* pNextMemoryTable = NULL;
  csd_vs_session_item_t* psess = NULL;

  psess = ( csd_vs_session_item_t* ) csd_voc_common_obtain_session_from_handle ( handle, &csd_vs_apr_objmgr );

  if ( NULL != psess )
  {
    /* Destroy server session */
    if (FALSE == csd_handle_is_ssr(handle))
    {
      if( psess->b_calibration_done == TRUE )
      {
        uint32_t cmd = VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA;

        /* Deregister Mapped PMEM Stream Calibration Data */
        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psess->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psess->h_cvs,
                                     0,
                                     cmd,
                                     APR_NULL_V,
                                     0 );

        CSD_MSG( CSD_PRIO_HIGH,
                 "csd_vs_close: VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA sent, apr_rc[0x%x]",
                 apr_rc);
        /* Wait for Deregister Response */
        if ( APR_EOK == apr_rc )
        {
          apr_rc = csd_event_wait ( psess->csd_vs_event );
        }

        if ( APR_EOK != apr_rc )
        {
          CSD_MSG( CSD_PRIO_ERROR,
                "csd_vs_close: csd_event_wait failed when waiting for Deregister Response, apr_rc[0x%x]",
                apr_rc);
        }

        if( ( APR_EOK == apr_rc ) &&
            ( csd_vs_active_sess_list.size == 1 ) )
        {
          /* Unmap deregistered PMEM Stream Calibration Data */
          cvs_cmd.mem_handle = csd_vs_mapped_pmem_handle;

          apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                       ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vs_src_addr,
                                       ( uint16_t )psess->h_apr_obj,
                                       csd_vs_mvm_dest_addr,
                                       APR_NULL_V,
                                       0,
                                       ( uint32_t )VSS_IMEMORY_CMD_UNMAP,
                                       (void*)&cvs_cmd,
                                       ( uint32_t )sizeof( vss_imemory_cmd_unmap_t ) );
          CSD_MSG( CSD_PRIO_HIGH,
                 "csd_vs_close: VSS_IMEMORY_CMD_UNMAP sent, apr_rc[0x%x]",
                 apr_rc);

          if ( APR_EOK == apr_rc )
          {
            /* Wait for Unmap Response */
            apr_rc = csd_event_wait ( psess->csd_vs_event );
          }

          /*  Deallocate memory after unmap */
          if ( APR_EOK != apr_rc )
          {
            CSD_MSG( CSD_PRIO_ERROR,
                 "csd_vs_close: csd_event_wait failed when waiting for Unmap Response, apr_rc[0x%x]",
                 apr_rc);
          }

          if ( csd_mem_free ( buffer_memory_handle ) )
          {
            CSD_MSG(CSD_PRIO_ERROR, "csd_vs_close: Error freeing memory" );
          }

          /* Freeing allocated memory_table memory */
          pMemoryTable = ( vss_imemory_table_t* )csd_vs_pVAMemoryTable;

          do
          {
            if ( pMemoryTable->next_table_descriptor.mem_address )
            {
              vss_imemory_table_t** ppNextMemoryTable;

              ppNextMemoryTable = ( vss_imemory_table_t** )( pMemoryTable + ( CSD_MEM_PAGE_SIZE - sizeof ( vss_imemory_block_t ) ) );

              pNextMemoryTable = ( vss_imemory_table_t* )*ppNextMemoryTable;
            }
            else
            {
              pNextMemoryTable = NULL;
            }

            if ( csd_mem_free ( table_memory_handle ) )
            {
              CSD_MSG(CSD_PRIO_ERROR, "csd_vs_close: Error freeing memory table" );
            }

            pMemoryTable = pNextMemoryTable;
          } while ( pNextMemoryTable );
        }
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }

      /* Un-map and free VoIP Buffer if it exists */
      if ( ( CSD_EOK == rc ) &&
           ( NULL != psess->voip_pkt_cfg.mapped_pmem_handle ) )
      {
        cvs_cmd.mem_handle = psess->voip_pkt_cfg.mapped_pmem_handle;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                      ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                      csd_vs_src_addr,
                                      ( uint16_t )psess->h_apr_obj,
                                      csd_vs_mvm_dest_addr,
                                      APR_NULL_V,
                                      0,
                                      ( uint32_t )VSS_IMEMORY_CMD_UNMAP,
                                      (void*)&cvs_cmd,
                                      ( uint32_t )sizeof( vss_imemory_cmd_unmap_t ) );
        CSD_MSG( CSD_PRIO_HIGH,
                "csd_vs_close: VSS_IMEMORY_CMD_UNMAP sent, apr_rc[0x%x]",
                apr_rc);

        if ( APR_EOK == apr_rc )
        {
          /* Wait for Unmap Response */
          ( void ) csd_event_wait ( psess->csd_vs_event );
        }

        rc = csd_map_apr_error_to_csd ( apr_rc );
        /*  Deallocate memory after unmap */
        if ( csd_mem_free ( psess->voip_pkt_cfg.pBufHandle ) )
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_vs_close: Error freeing memory" );
        }

        /* Freeing one imemory block used for the VoIP Buffer */
        pMemoryTable = ( vss_imemory_table_t* )psess->voip_pkt_cfg.pVAMemoryTable;

        if ( csd_mem_free ( psess->voip_pkt_cfg.pTableMemHandle ) )
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_vs_close: Error freeing memory table" );
        }
      }

      if ( CSD_EOK == rc )
      {
        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                      ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                      csd_vs_src_addr,
                                      ( uint16_t )psess->h_apr_obj,
                                      csd_vs_cvs_dest_addr,
                                      ( uint16_t )psess->h_cvs,
                                      ( uint32_t )NULL,
                                      ( uint32_t )APRV2_IBASIC_CMD_DESTROY_SESSION,
                                          NULL, 0 );
        CSD_MSG( CSD_PRIO_HIGH,
                    "csd_vs_close: APRV2_IBASIC_CMD_DESTROY_SESSION sent, apr_rc[0x%x]",
                    apr_rc);

        if ( APR_EOK == apr_rc ) /* what about checking for errors */
        {
          apr_rc = csd_event_wait ( psess->csd_vs_event );
        }

        if ( APR_EOK != apr_rc )
        {
          CSD_MSG( CSD_PRIO_ERROR,
                "csd_vs_close: csd_event_wait failed when waiting for APR Destroy Session, apr_rc[0x%x]",
                apr_rc);
        }
        rc = csd_map_apr_error_to_csd ( apr_rc );
        if( rc == CSD_EOK )
        {
          csd_vs_update_state (psess, APRV2_IBASIC_CMD_DESTROY_SESSION);
        }
      }
    }

    if( ( csd_vs_active_sess_list.size == 1 ) &&
        ( rc == CSD_EOK ) )
    {
      CSD_MSG( CSD_PRIO_LOW,
                     "csd_vs_close: no client for stream, clean up all variables");
      csd_vs_va_cal = NULL;
      csd_vs_pa_cal = 0;
      csd_vs_pVAMemoryTable = NULL;
      csd_vs_mapped_pmem_handle = 0;
      csd_vs_cal_buf_size = 0;
      csd_vs_cal_size = 0;
      csd_vs_memory_blocks = 0;
      table_memory_handle = NULL;
      buffer_memory_handle = NULL;
    }

    /* free resources associated with session */
    ( void ) csd_event_destroy ( psess->csd_vs_event );
    ( void ) csd_handle_free ( psess->h_csd );
    ( void ) apr_objmgr_free_object ( &csd_vs_apr_objmgr, psess->h_apr_obj );
    ( void ) apr_list_delete ( &csd_vs_active_sess_list, &( psess->link ) );
    ( void ) apr_list_add_tail( &csd_vs_free_sess_list, &( psess->link ) );
  }
  else
  {
    rc = CSD_EBADPARAM;
  }

  return rc;
}

CSD_INTERNAL int32_t csd_vs_ioctl (
  uint32_t handle,
  uint32_t code,
  void* params,
  uint32_t size )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vs_session_item_t  *psession = NULL;

  if ( NULL == params )
  {
    return CSD_EBADPARAM;
  }

  /* extract psession */
  psession = csd_voc_common_obtain_session_from_handle ( handle, &csd_vs_apr_objmgr );

  if ( NULL != psession )
  {
    switch ( code )
    {
    case CSD_VS_IOCTL_GET_CVS_HANDLE: /* internal IOCTL */
      {
        csd_vs_ioctl_get_cvs_handle_t *cvs_handle;

        cvs_handle = ( csd_vs_ioctl_get_cvs_handle_t* ) params;

        *cvs_handle->ret_cvs_handle = psession->h_cvs;

        if ( !*cvs_handle->ret_cvs_handle )
        {
          rc = CSD_ENORESOURCE;
        }
      }
      break;

/**** Future API for B-family ****/
#if 0
    case CSD_VS_IOCTL_SET_VAR_VOC_SAMPLING_RATE:
      {
        csd_vs_ioctl_set_var_voc_sampling_rate_t* csd_vs_set_var_voc_sampling = ( csd_vs_ioctl_set_var_voc_sampling_rate_t* ) params;
        vss_istream_cmd_set_var_voc_sampling_rate_t cvs_cmd;

        cvs_cmd.rx = csd_vs_set_var_voc_sampling->rx;
        cvs_cmd.tx = csd_vs_set_var_voc_sampling->tx;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_var_voc_sampling->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_set_var_voc_sampling_rate_t )
                                   );

        CSD_MSG( CSD_PRIO_LOW,
                       "csd_vs_ioctl: CSD_VS_IOCTL_SET_VAR_VOC_SAMPLING_RATE, apr_rc[0x%x] token = [0x%x]",
                       apr_rc, csd_vs_set_var_voc_sampling->cmd_token );
		rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;
#endif

    case CSD_VS_IOCTL_SET_MEDIA_TYPE:
      {
        csd_vs_ioctl_set_media_type_t* csd_vs_set_media = ( csd_vs_ioctl_set_media_type_t* ) params;
        vss_istream_cmd_set_media_type_t cvs_cmd;

        cvs_cmd.rx_media_id = csd_vs_set_media->rx_media_id;
        cvs_cmd.tx_media_id = csd_vs_set_media->tx_media_id;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_media->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_SET_MEDIA_TYPE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_set_media_type_t )
                                   );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: CSD_VS_IOCTL_SET_MEDIA_TYPE, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_media->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_ENCODER_DTX_MODE:
      {
        csd_vs_ioctl_set_encoder_dtx_mode_t *csd_vs_set_encoder_dtx_type = ( csd_vs_ioctl_set_encoder_dtx_mode_t* ) params;
        vss_istream_cmd_set_enc_dtx_mode_t cvs_cmd;

        cvs_cmd.enable = csd_vs_set_encoder_dtx_type->enable;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_encoder_dtx_type->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_set_enc_dtx_mode_t )
                                   );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_SET_ENC_DTX_MODE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_encoder_dtx_type->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_DEC_TIMEWARP:
      {
        csd_vs_ioctl_set_dec_timewarp_t *csd_vs_set_dec_timewarp = ( csd_vs_ioctl_set_dec_timewarp_t* ) params;
        vss_istream_cmd_set_dec_timewarp_t cvs_cmd;

        cvs_cmd.enable_phase_match = csd_vs_set_dec_timewarp->enable_phase_match;
        cvs_cmd.enable_time_warp = csd_vs_set_dec_timewarp->enable_time_warp;
        cvs_cmd.exp_length = csd_vs_set_dec_timewarp->factor;
        cvs_cmd.phase_offset = csd_vs_set_dec_timewarp->phase_offset;
        cvs_cmd.run_length = csd_vs_set_dec_timewarp->run_length;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_dec_timewarp->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_SET_DEC_TIMEWARP,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_set_dec_timewarp_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_SET_DEC_TIMEWARP sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_dec_timewarp->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_ENC_MINMAX_RATE:
      {
        csd_vs_ioctl_set_enc_minmax_rate_t *csd_vs_set_enc_minmax_rate = ( csd_vs_ioctl_set_enc_minmax_rate_t* ) params;
        vss_istream_cmd_cdma_set_enc_minmax_rate_t cvs_cmd;

        cvs_cmd.min_rate = csd_vs_set_enc_minmax_rate->min_rate;
        cvs_cmd.max_rate = csd_vs_set_enc_minmax_rate->max_rate;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_enc_minmax_rate->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_cdma_set_enc_minmax_rate_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_enc_minmax_rate->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_ENC_RATE_MODULATION:
      {
        csd_vs_ioctl_set_enc_rate_modulation_t *csd_vs_set_enc_rate_mod = ( csd_vs_ioctl_set_enc_rate_modulation_t* ) params;
        vss_istream_cmd_cdma_set_enc_rate_modulation_t cvs_cmd;

        cvs_cmd.mode = csd_vs_set_enc_rate_mod->mode;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_enc_rate_mod->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_cdma_set_enc_rate_modulation_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_enc_rate_mod->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE:
      {
        csd_vs_ioctl_voc_qcelp13k_set_rate_t *csd_vs_set_qcelp13k_set_rate = ( csd_vs_ioctl_voc_qcelp13k_set_rate_t* ) params;
        vss_istream_cmd_voc_qcelp13k_set_rate_t cvs_cmd;

        cvs_cmd.rate = csd_vs_set_qcelp13k_set_rate->rate;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_qcelp13k_set_rate->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_voc_qcelp13k_set_rate_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_qcelp13k_set_rate->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_VOC_4GVNB_SET_RATE:
      {
        csd_vs_ioctl_voc_4gvnb_set_rate_t *csd_vs_set_4gvnb_set_rate = ( csd_vs_ioctl_voc_4gvnb_set_rate_t* ) params;
        vss_istream_cmd_voc_4gvnb_set_rate_t cvs_cmd;

        cvs_cmd.rate = csd_vs_set_4gvnb_set_rate->rate;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_4gvnb_set_rate->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_voc_4gvnb_set_rate_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_4gvnb_set_rate->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_VOC_4GVWB_SET_RATE:
      {
        csd_vs_ioctl_voc_4gvwb_set_rate_t *csd_vs_set_4gvwb_set_rate = ( csd_vs_ioctl_voc_4gvwb_set_rate_t* ) params;
        vss_istream_cmd_voc_4gvwb_set_rate_t cvs_cmd;

        cvs_cmd.rate = csd_vs_set_4gvwb_set_rate->rate;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_4gvwb_set_rate->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_voc_4gvwb_set_rate_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_4gvwb_set_rate->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE:
      {
        csd_vs_ioctl_voc_amr_set_enc_rate_t *csd_vs_amr_set_enc = ( csd_vs_ioctl_voc_amr_set_enc_rate_t* ) params;
        vss_istream_cmd_voc_amr_set_enc_rate_t cvs_cmd;

        cvs_cmd.mode = csd_vs_amr_set_enc->mode;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_amr_set_enc->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_voc_amr_set_enc_rate_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_amr_set_enc->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );

      }
      break;

    case CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE:
      {
        csd_vs_ioctl_voc_amrwb_set_enc_rate_t *csd_vs_amrwb_set_enc = ( csd_vs_ioctl_voc_amrwb_set_enc_rate_t* ) params;
        vss_istream_cmd_voc_amrwb_set_enc_rate_t cvs_cmd;

        cvs_cmd.mode = csd_vs_amrwb_set_enc->mode;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_amrwb_set_enc->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_voc_amrwb_set_enc_rate_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_amrwb_set_enc->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_MUTE_V2:
      {
        csd_vs_ioctl_set_mute_v2_t     *csd_vs_mute_v2 = ( csd_vs_ioctl_set_mute_v2_t* ) params;
        vss_ivolume_cmd_mute_v2_t  cvs_cmd;

        cvs_cmd.direction  = csd_vs_mute_v2->direction;
        cvs_cmd.mute_flag  = csd_vs_mute_v2->mute_flag;
        cvs_cmd.ramp_duration_ms  = csd_vs_mute_v2->ramp_duration;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_mute_v2->cmd_token,
                                     ( uint32_t )VSS_IVOLUME_CMD_MUTE_V2,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_ivolume_cmd_mute_v2_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_IVOLUME_CMD_MUTE_V2 sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_mute_v2->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_UI_PROPERTY: /* One or more */
      {
        csd_vs_ioctl_set_ui_property_t      *csd_vs_prop = ( csd_vs_ioctl_set_ui_property_t* )params;
        vss_icommon_cmd_set_ui_property_t   cvs_cmd;
        aprv2_packet_t* papr_packet = NULL;

        cvs_cmd.module_id  = csd_vs_prop->module_id;
        cvs_cmd.param_id   = csd_vs_prop->param_id;
        cvs_cmd.param_size = csd_vs_prop->param_size;
        cvs_cmd.reserved   = 0;

        apr_rc = __aprv2_cmd_alloc_ext( csd_vs_apr_handle,
                                    ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                    csd_vs_src_addr,
                                    ( uint16_t )psession->h_apr_obj,
                                    csd_vs_cvs_dest_addr,
                                    ( uint16_t )psession->h_cvs,
                                    ( uint32_t )csd_vs_prop->cmd_token,
                                    ( uint32_t )VSS_ICOMMON_CMD_SET_UI_PROPERTY,
                                    sizeof( vss_icommon_cmd_set_ui_property_t ) + cvs_cmd.param_size,
                                    &papr_packet );

        if( ( APR_EOK == apr_rc) && ( papr_packet != NULL ) )
        { /* copy the param_data into apr buffer */
          uint8_t* pdata;
          uint32_t psize;
          pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
          psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(papr_packet->header);
          csd_memscpy( pdata, psize,
                       &cvs_cmd, sizeof ( vss_icommon_cmd_set_ui_property_t ) ); /* copy the cmd structure */
          pdata += sizeof(vss_icommon_cmd_set_ui_property_t );
          psize -= sizeof(vss_icommon_cmd_set_ui_property_t );
          csd_memscpy( pdata, psize ,
                       csd_vs_prop->param_data, csd_vs_prop->param_size );

          apr_rc = __aprv2_cmd_forward( csd_vs_apr_handle, papr_packet );
          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ICOMMON_CMD_SET_UI_PROPERTY sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_prop->cmd_token );
        }
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_GET_UI_PROPERTY:/* One at a time */
      {
        csd_vs_ioctl_get_ui_property_t *csd_vs_prop = ( csd_vs_ioctl_get_ui_property_t* )params;
        vss_icommon_cmd_get_ui_property_t cvs_cmd;
        csd_vs_cmd_list_item_t* pcmd = NULL; /* use this to keep track of the buffers*/

        apr_rc = apr_list_remove_head( &csd_vs_free_cmd_list, ( apr_list_node_t** ) &pcmd );

        if ( ( APR_EOK == apr_rc ) && ( NULL != pcmd ) )
        {
          csd_memscpy( &pcmd->cmd, sizeof(pcmd->cmd),
                       csd_vs_prop, sizeof ( csd_vs_ioctl_get_ui_property_t ) );
          pcmd->cmd_token = token_counter;
          apr_rc = apr_list_add_tail( &csd_vs_track_dispatched_cmd_list, &pcmd->link );

          cvs_cmd.module_id  = csd_vs_prop->module_id;
          cvs_cmd.param_id   = csd_vs_prop->param_id;
          cvs_cmd.param_size = csd_vs_prop->param_size;

          apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                       ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vs_src_addr,
                                       ( uint16_t )psession->h_apr_obj,
                                       csd_vs_cvs_dest_addr,
                                       ( uint16_t )psession->h_cvs,
                                       ( uint32_t )token_counter++,
                                       ( uint32_t )VSS_ICOMMON_CMD_GET_UI_PROPERTY,
                                       (void*) &cvs_cmd,
                                       ( uint32_t )sizeof( cvs_cmd )
                                     );
          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ICOMMON_CMD_GET_UI_PROPERTY sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_prop->cmd_token );
          if ( APR_EOK == apr_rc )
          {
            apr_rc = csd_event_wait ( psession->csd_vs_event  );
          }
        }
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

      case CSD_VS_IOCTL_START_RECORD:
      {
        csd_vs_ioctl_start_record_t *csd_vs_start_record = ( csd_vs_ioctl_start_record_t* ) params;
        vss_istream_cmd_start_record_t cvs_cmd;

        cvs_cmd.rx_tap_point = csd_vs_start_record->rx_tap_point;
        cvs_cmd.tx_tap_point = csd_vs_start_record->tx_tap_point;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     (uint16_t)psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     (uint16_t)psession->h_cvs,
                                     (uint32_t)csd_vs_start_record->cmd_token,
                                     (uint32_t)VSS_ISTREAM_CMD_START_RECORD,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_start_record_t ) );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_START_RECORD sent, apr_rc[0x%x] token[0x%x] , rx_tap_point[0x%x], tx_tap_point[0x%x]",
                       apr_rc, csd_vs_start_record->cmd_token, cvs_cmd.rx_tap_point, cvs_cmd.tx_tap_point );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_START_RECORD_V2:
      {
        csd_vs_ioctl_start_record_v2_t *csd_vs_start_record = ( csd_vs_ioctl_start_record_v2_t* ) params;
        vss_irecord_cmd_start_t cvs_cmd;

        cvs_cmd.rx_tap_point = csd_vs_start_record->rx_tap_point;
        cvs_cmd.tx_tap_point = csd_vs_start_record->tx_tap_point;
        cvs_cmd.mode = csd_vs_start_record->mode;

        if ( csd_vs_start_record->dev_id != VSS_IRECORD_PORT_ID_DEFAULT )
        {
        rc = csd_voc_commmon_get_active_afe_port_id ( csd_vs_start_record->dev_id,
                                                      (uint16_t*)&cvs_cmd.port_id );
        }
        else
        {
          rc = CSD_EOK;
          cvs_cmd.port_id = csd_vs_start_record->dev_id;
        }

        if ( CSD_EOK == rc )
        {
          apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                       ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vs_src_addr,
                                       ( uint16_t )psession->h_apr_obj,
                                       csd_vs_cvs_dest_addr,
                                       ( uint16_t )psession->h_cvs,
                                       ( uint32_t )csd_vs_start_record->cmd_token,
                                       ( uint32_t )VSS_IRECORD_CMD_START,
                                       (void*) &cvs_cmd,
                                       ( uint32_t )sizeof( vss_irecord_cmd_start_t ) );

           CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_IRECORD_CMD_START sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_start_record->cmd_token );
          rc = csd_map_apr_error_to_csd ( apr_rc );
        }
      }
      break;

    case CSD_VS_IOCTL_STOP_RECORD:
      {
        uint32_t* cmd_token = ( uint32_t* )params;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )*cmd_token,
                                     ( uint32_t )VSS_IRECORD_CMD_STOP,
                                     NULL,
                                     0
                                   );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_IRECORD_CMD_STOP sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, ( uint32_t )*cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

      case CSD_VS_IOCTL_START_PLAYBACK:
      {
        uint32_t* cmd_token = ( uint32_t* )params;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     (uint8_t)APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     (uint16_t)psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     (uint16_t)psession->h_cvs,
                                     (uint32_t)*cmd_token,
                                     (uint32_t)VSS_ISTREAM_CMD_START_PLAYBACK,
                                     NULL,
                                     0 );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_START_PLAYBACK sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, ( uint32_t )*cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_START_PLAYBACK_V2:
      {
        csd_vs_ioctl_start_playback_v2_t *csd_vs_start_playback = ( csd_vs_ioctl_start_playback_v2_t* ) params;
        vss_iplayback_cmd_start_t cvs_cmd;

        if ( csd_vs_start_playback->dev_id != VSS_IPLAYBACK_PORT_ID_DEFAULT )
        {
        rc = csd_voc_commmon_get_active_afe_port_id ( csd_vs_start_playback->dev_id,
                                                      (uint16_t*)&cvs_cmd.port_id );
        }
        else
        {
          rc = CSD_EOK;
          cvs_cmd.port_id = csd_vs_start_playback->dev_id;
        }

        if ( rc == CSD_EOK )
        {

          apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                       ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                       csd_vs_src_addr,
                                       ( uint16_t )psession->h_apr_obj,
                                       csd_vs_cvs_dest_addr,
                                       ( uint16_t )psession->h_cvs,
                                       ( uint32_t )csd_vs_start_playback->cmd_token,
                                       ( uint32_t )VSS_IPLAYBACK_CMD_START,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_iplayback_cmd_start_t ) );

          CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_IPLAYBACK_CMD_START sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, ( uint32_t )csd_vs_start_playback->cmd_token );
          rc = csd_map_apr_error_to_csd ( apr_rc );
        }
      }
      break;

    case CSD_VS_IOCTL_STOP_PLAYBACK:
      {
        uint32_t* cmd_token = ( uint32_t* )params;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )*cmd_token,
                                     ( uint32_t )VSS_IPLAYBACK_CMD_STOP,
                                     NULL,
                                     0 );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_IPLAYBACK_CMD_STOP sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, ( uint32_t )*cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_DTMF_GENERATION:
      {
        csd_vs_ioctl_set_dtmf_generation_t *csd_vs_set_dtmf_gen = ( csd_vs_ioctl_set_dtmf_generation_t* ) params;
        vss_istream_cmd_set_dtmf_generation_t cvs_cmd;

        cvs_cmd.direction = csd_vs_set_dtmf_gen->direction;
        cvs_cmd.mix_flag  = csd_vs_set_dtmf_gen->mix_flag;
        cvs_cmd.tone_1    = csd_vs_set_dtmf_gen->tone_1;
        cvs_cmd.tone_2    = csd_vs_set_dtmf_gen->tone_2;
        cvs_cmd.gain      = csd_vs_set_dtmf_gen->gain;
        cvs_cmd.duration  = csd_vs_set_dtmf_gen->duration;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_dtmf_gen->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_SET_DTMF_GENERATION,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_set_dtmf_generation_t )
                                   );
        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_SET_DTMF_GENERATION sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_dtmf_gen->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    case CSD_VS_IOCTL_SET_RX_DTMF_DETECTION:
      {
        csd_vs_ioctl_set_rx_dtmf_detection_t *csd_vs_set_rx_dtmf_detect = ( csd_vs_ioctl_set_rx_dtmf_detection_t* ) params;
        vss_istream_cmd_set_rx_dtmf_detection_t cvs_cmd;

        cvs_cmd.enable = csd_vs_set_rx_dtmf_detect->enable;

        apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                     ( uint8_t )APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     csd_vs_src_addr,
                                     ( uint16_t )psession->h_apr_obj,
                                     csd_vs_cvs_dest_addr,
                                     ( uint16_t )psession->h_cvs,
                                     ( uint32_t )csd_vs_set_rx_dtmf_detect->cmd_token,
                                     ( uint32_t )VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION,
                                     (void*) &cvs_cmd,
                                     ( uint32_t )sizeof( vss_istream_cmd_set_rx_dtmf_detection_t )
                                   );

        CSD_MSG( CSD_PRIO_HIGH,
                       "csd_vs_ioctl: VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION sent, apr_rc[0x%x] token[0x%x]",
                       apr_rc, csd_vs_set_rx_dtmf_detect->cmd_token );
        rc = csd_map_apr_error_to_csd ( apr_rc );
      }
      break;

    default: /* All others not supported for FC1 */
        rc = CSD_EUNSUPPORTED;
      break;
    } /* switch cmd */
  }
  else
  {
    rc = CSD_EHANDLE;
  }

  return rc;
}

CSD_INTERNAL int32_t csd_vs_read (
  uint32_t handle,
  void* params,
  uint32_t size )
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vs_session_item_t  *psess = NULL;
  csd_vs_read_t* vs_read = ( csd_vs_read_t* )params;
  int32_t ret_val = 0;

  /* extract psession */
  psess = csd_voc_common_obtain_session_from_handle ( handle, &csd_vs_apr_objmgr );

  if ( ( NULL != psess ) &&
       ( sizeof ( csd_vs_read_t ) == size ) )
  {
    if ( ( NULL != vs_read->pbuf ) &&
         ( vs_read->buf_size <= psess->stream_pkt_size ) )
    {
      uint8_t *pdata = psess->voip_pkt_cfg.pVABufEnc;
      csd_vs_voip_buffer_t voip_buf_debug_print;

      voip_buf_debug_print.timestamp = *(uint32 *) pdata;
      voip_buf_debug_print.media_type = *(uint32 *) (pdata + sizeof(uint32_t));
      voip_buf_debug_print.data_size = *(uint32 *) (pdata + ( 2 * sizeof(uint32_t) ));

      /* Return to client the media_type that the DSP is sending back */
      *vs_read->pmedia_id = psess->enc_type;

      /* Skip the non-payload data in the shard buffer */
      pdata += sizeof( csd_vs_voip_buffer_t );

      /*  Copy Shared buffer Encode data into the Client's buffer and send Encode Buffer Consumed Event to CVS  */
      csd_memscpy ( vs_read->pbuf, vs_read->buf_size, pdata, vs_read->buf_size );

      CSD_MSG(CSD_PRIO_LOW, "CSD VS => csd_vs_read(), shared ENC buffer timestamp[%d], media_type[%d], buf_size[%d]",
                    voip_buf_debug_print.timestamp, voip_buf_debug_print.media_type, voip_buf_debug_print.data_size);

      apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                  ( uint8_t )APRV2_PKT_MSGTYPE_EVENT_V,
                                  csd_vs_src_addr,
                                  ( uint16_t )psess->h_apr_obj,
                                  csd_vs_cvs_dest_addr,
                                  ( uint16_t )psess->h_cvs,
                                  0,
                                  ( uint32_t )VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED,
                                  APR_NULL_V,
                                  0);
      CSD_MSG( CSD_PRIO_HIGH,
                       "VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED sent, apr_rc[0x%x]",
                       apr_rc);
      rc = csd_map_apr_error_to_csd ( apr_rc );
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
  }

  if ( CSD_EBADPARAM != rc )
  {
    ret_val = vs_read->buf_size;
  }
  else
  {
    ret_val = -1;
  }

  return ret_val;
}


CSD_INTERNAL int32_t csd_vs_write (
  uint32_t handle,
  void* params,
  uint32_t size)

{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  csd_vs_session_item_t  *psess = NULL;
  csd_vs_write_t* vs_write = ( csd_vs_write_t* )params;
  csd_vs_voip_buffer_t voip_buf_info;
  int32_t write_count = 0;

  /* extract psession */
  psess = csd_voc_common_obtain_session_from_handle ( handle, &csd_vs_apr_objmgr );

  if ( ( NULL != psess ) &&
       ( sizeof ( csd_vs_write_t ) == size ) )
  {
    if ( ( NULL != vs_write->pbuf ) &&
         ( vs_write->media_id == psess->dec_type ) &&
         ( vs_write->buf_size <= psess->stream_pkt_size ) )
    {
      uint8_t* pdata = psess->voip_pkt_cfg.pVABufDec;
      uint32_t psize = psess->voip_pkt_cfg.buf_size/2;
      csd_vs_voip_buffer_t voip_buf_debug_print;

      /*  Fill in shared buffer info before writing payload */
      voip_buf_info.timestamp = 0;	// Timestamp = 0 for now.  Client should fill this info in?
      voip_buf_info.media_type = vs_write->media_id;
      voip_buf_info.data_size = vs_write->buf_size;

      voip_buf_debug_print.timestamp = *(uint32 *) pdata;
      voip_buf_debug_print.media_type = *(uint32 *) (pdata + sizeof(uint32_t));
      voip_buf_debug_print.data_size = *(uint32 *) (pdata + ( 2 * sizeof(uint32_t) ));

      csd_memscpy ( pdata, psize, &voip_buf_info, sizeof (csd_vs_voip_buffer_t) );
      pdata += sizeof( csd_vs_voip_buffer_t );
      psize -= sizeof( csd_vs_voip_buffer_t );

      /*  Copy Client packet payload data into shared decode buffer and send Dec Buffer Ready Event to CVS  */
      csd_memscpy ( pdata, psize, vs_write->pbuf, vs_write->buf_size );

      CSD_MSG(CSD_PRIO_LOW, "shared DEC buffer timestamp[%d], media_type[%d], buf_size[%d]",
                    voip_buf_debug_print.timestamp, voip_buf_debug_print.media_type, voip_buf_debug_print.data_size);

      apr_rc = __aprv2_cmd_alloc_send( csd_vs_apr_handle,
                                  ( uint8_t )APRV2_PKT_MSGTYPE_EVENT_V,
                                  csd_vs_src_addr,
                                  ( uint16_t )psess->h_apr_obj,
                                  csd_vs_cvs_dest_addr,
                                  ( uint16_t )psess->h_cvs,
                                  0,
                                  ( uint32_t )VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_READY,
                                  APR_NULL_V,
                                  0);
      CSD_MSG( CSD_PRIO_HIGH,
                       "VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_READY sent, apr_rc[0x%x]",
                       apr_rc);
      rc = csd_map_apr_error_to_csd ( apr_rc );
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
  }

  if ( CSD_EOK == rc )
  {
    write_count = vs_write->buf_size;
  }
  return write_count;
}

/*
 ** End External APIs
*/
