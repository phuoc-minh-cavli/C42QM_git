/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S _ V S . C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file for voice 
  stream.


Copyright (c) 2011-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services_vs.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/15/16    rk     Abstracting OSAL layer
09/01/14    rp     replaced the qmi debug buffer with uniform csd debug buffer
07/03/13    vk     Changes to remove compiler warnings
06/26/12    jk     Added support for not calling callback when client is disconnected 
12/06/11    jp     Added music delivery support
08/16/11    jp     Incorporated review comment
06/13/11    jg     Created file (initial version)

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "csd_qmi_services_vs.h"
#include "csd_vs_ioctl.h"
#include "csd_vs.h"
#include "csd_debug_info.h"
#include "apr_errcodes.h"
#include "csd_os_dependencies.h"
#include "csdlite.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/* This varaible is used to store the QMI client info. When multiple sessions
    of VS is called, the variable will be overwritten erasing the other session
    client info. we will have to handle this senario properly*/
static csd_cb_data_type csd_vs_cb_data;
extern csd_event_t qmi_csd_sync_event;
/*=============================================================================
                    QMI CSD Voice Stream Callback Functions
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_vs_callback
=============================================================================*/
/*!
@brief
  This function is called when voice stream event happens.

  @param[in] cmd_token  Transaction identifier provided by the
                        client side that allows the client to
                        identify which command completed.
  @param[in] event_id   Callback event ID.
  @param[in] param      Pointer to the payload data of the callback event.
  @param[in] param_size Size of the payload data for the callback event.
  @param[in] cb_data    Pointer to the callback data.

*/
/*=========================================================================*/

uint32_t qmi_csd_vs_callback(uint32_t cmd_token, uint32_t event_id,
                         void* param, uint32_t param_size, void* cb_data_info)
{
  uint32_t rc = CSD_EOK, hdl, size;
  csd_cmd_token_param_t cmd_param_info;
  qmi_client_handle clnt_hdl;
  csd_vs_ioctl_get_ui_property_t *svc_csd_vs_param;
  qmi_req_handle req_handle;

  uint32_t cmd_param_cmd_type = 0, cmd_param_cmd_token = 0;
  int32_t msg_id, resp_len = 0;
  qmi_csi_error resp_err;

  qmi_csd_vs_cmd_resp_msg *resp_msg;

  csd_vs_event_dtmf_generation_ended_t* rsp;
  csd_vs_event_rx_dtmf_detected_t* rsp1;

  qmi_csd_ioctl_vs_dtmf_generation_ended_ind_msg_v01 ind_info;
  qmi_csd_ioctl_vs_rx_dtmf_detected_ind_msg_v01 ind_info1;

  qmi_csd_vs_ioctl_get_ui_property_resp_v01 *ptr;
  csd_cb_data_type *cb_data_ptr = (csd_cb_data_type*)cb_data_info;
  
  hdl = cb_data_ptr->handle;
  clnt_hdl = (cb_data_ptr->clnt_info_data)->clnt;

  switch( event_id ) 
  {
    /* cmd response */
    case CSD_VS_EVENTID_COMMAND_RSP:
    {
      cmd_param_info.cmd_token = cmd_token;

      rc = qmi_csd_client_info_get_list ( &cmd_param_info);
      if (CSD_EOK == rc)
      {
        rc = ((csd_vs_event_command_rsp_t*)param)->status_id;
        cmd_param_cmd_type = cmd_param_info.cmd_type;
        cmd_param_cmd_token = cmd_param_info.cmd_token;

        req_handle = cmd_param_info.req_handle;
        msg_id = cmd_param_info.msg_id;

        resp_msg = (qmi_csd_vs_cmd_resp_msg*)malloc(sizeof(qmi_csd_vs_cmd_resp_msg));	   

        if( resp_msg != NULL )
        {
          memset( resp_msg, 0, sizeof(*resp_msg) );

          switch( cmd_param_cmd_type ) 
          {
            case CSD_VS_IOCTL_SET_MUTE:
            {
              resp_len = sizeof (resp_msg->u.qmi_csd_set_mute_resp);
              
              if( rc != CSD_EOK )
              {
                resp_msg->u.qmi_csd_set_mute_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_mute_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_mute_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_mute_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_MUTE failed, rc[0x%x]", rc );
              }
              else
              {
                resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_mute_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_mute_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_mute_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_mute_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_MUTE success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_SET_DTMF_GENERATION:
            {
              resp_len = sizeof (resp_msg->u.qmi_csd_dtmf_gen_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_dtmf_gen_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_dtmf_gen_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_dtmf_gen_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_dtmf_gen_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_dtmf_gen_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_dtmf_gen_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_DTMF_GENERATION failed, rc[0x%x]", rc );
              }
              else 
              {
                resp_msg->u.qmi_csd_dtmf_gen_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_dtmf_gen_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_dtmf_gen_resp.handle = hdl;
                resp_msg->u.qmi_csd_dtmf_gen_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_dtmf_gen_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_DTMF_GENERATION success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_SET_RX_DTMF_DETECTION:
            {
              resp_len = sizeof ( resp_msg->u.qmi_csd_rx_dtmf_dect_resp);

              if( rc != CSD_EOK )
              {
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_RX_DTMF_DETECTION failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.handle = hdl;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_rx_dtmf_dect_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_RX_DTMF_DETECTION success" );
              }   
            }
            break;
          
            case CSD_VS_IOCTL_SET_UI_PROPERTY:
            {
              resp_len = sizeof (qmi_csd_ioctl_vs_cmd_set_ui_property_resp_msg_v01);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_ui_prop_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_ui_prop_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_ui_prop_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_ui_prop_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_ui_prop_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_ui_prop_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_UI_PROPERTY failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_ui_prop_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_ui_prop_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_ui_prop_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_ui_prop_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_ui_prop_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_UI_PROPERTY success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_GET_UI_PROPERTY:
            {
              /* the memory pointed by *resp (or cmd_param_info->cmd_resp_ptr) has
                 been allocated in reqest message handler.
                 The memory pointed by svc_csd_vs_param->param_data, and
                 svc_csd_vs_param have also been allocated in request handler.  */
              resp_len = sizeof (resp_msg->u.qmi_csd_get_ui_prop_resp);
              if( rc != CSD_EOK )
              {
                resp_msg->u.qmi_csd_get_ui_prop_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_get_ui_prop_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_get_ui_prop_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_get_ui_prop_resp.cmd_token_valid = FALSE;
                resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_vs_ioctl_get_ui_property_resp_payload_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_GET_UI_PROPERTY failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_get_ui_prop_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_get_ui_prop_resp.handle = hdl;
                resp_msg->u.qmi_csd_get_ui_prop_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_get_ui_prop_resp.cmd_token = cmd_param_cmd_token;
                resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_vs_ioctl_get_ui_property_resp_payload_valid = TRUE;
                
                /* fill in data payload */
                svc_csd_vs_param = (csd_vs_ioctl_get_ui_property_t*) param;
                ptr = (qmi_csd_vs_ioctl_get_ui_property_resp_v01*) (&(resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_vs_ioctl_get_ui_property_resp_payload));
                /* svc_csd_vs_param now point to memory with UI property structure filled in. */
                size = svc_csd_vs_param->param_size;
                ptr->cmd_token = cmd_param_cmd_token;
                ptr->module_id = svc_csd_vs_param->module_id;
                ptr->param_id = svc_csd_vs_param->param_id ;
                ptr->param_data_len = size ;
                csd_memscpy( ptr->param_data, sizeof(ptr->param_data),
                             svc_csd_vs_param->param_data, size );
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_GET_UI_PROPERTY success" );
              }     
            }
            break;
          
            case CSD_VS_IOCTL_SET_MEDIA_TYPE:
            {
              resp_len = sizeof ( resp_msg->u.qmi_csd_set_media_type_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_media_type_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_media_type_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_media_type_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_media_type_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_media_type_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_media_type_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_MEDIA_TYPE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_media_type_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_media_type_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_media_type_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_media_type_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_media_type_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_MEDIA_TYPE success" );
              } 
            } 
            break;
          
            case CSD_VS_IOCTL_START_RECORD:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_start_record_resp);
            
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_start_record_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_start_record_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_start_record_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_start_record_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_start_record_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_start_record_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_START_RECORD failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_start_record_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_start_record_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_start_record_resp.handle = hdl;
                resp_msg->u.qmi_csd_start_record_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_start_record_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_START_RECORD success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_STOP_RECORD:
            {
              resp_len = sizeof ( resp_msg->u.qmi_csd_stop_record_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_stop_record_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_stop_record_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_stop_record_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_stop_record_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_stop_record_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_stop_record_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_STOP_RECORD failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_stop_record_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_stop_record_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_stop_record_resp.handle = hdl;
                resp_msg->u.qmi_csd_stop_record_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_stop_record_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_STOP_RECORD success" );
              } 
            }  
            break;
          
            case CSD_VS_IOCTL_START_PLAYBACK:
            {
              resp_len = sizeof ( resp_msg->u.qmi_csd_start_playback_resp);

              if( rc != CSD_EOK )
              {
                resp_msg->u.qmi_csd_start_playback_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_start_playback_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_start_playback_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_start_playback_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_start_playback_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_start_playback_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_START_PLAYBACK failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_start_playback_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_start_playback_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_start_playback_resp.handle = hdl;
                resp_msg->u.qmi_csd_start_playback_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_start_playback_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_START_PLAYBACK success" );
              }
            } 
            break;
          
            case CSD_VS_IOCTL_STOP_PLAYBACK:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_stop_playback_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_stop_playback_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_stop_playback_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_stop_playback_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_stop_playback_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_stop_playback_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_stop_playback_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_STOP_PLAYBACK failed, rc[0x%x]", rc );
              }
              else 
              {
                resp_msg->u.qmi_csd_stop_playback_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_stop_playback_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_stop_playback_resp.handle = hdl;
                resp_msg->u.qmi_csd_stop_playback_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_stop_playback_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_STOP_PLAYBACK success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_SET_ENCODER_DTX_MODE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_enc_dtx_mode_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_ENCODER_DTX_MODE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.handle = hdl;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_enc_dtx_mode_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_ENCODER_DTX_MODE success" );
              } 
            }
            break;
          
            case CSD_VS_IOCTL_SET_DEC_TIMEWARP:
            {
              resp_len = sizeof ( resp_msg->u.qmi_csd_dec_timewarp_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_dec_timewarp_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_dec_timewarp_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_dec_timewarp_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_dec_timewarp_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_dec_timewarp_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_dec_timewarp_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_DEC_TIMEWARP failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_dec_timewarp_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_dec_timewarp_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_dec_timewarp_resp.handle = hdl;
                resp_msg->u.qmi_csd_dec_timewarp_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_dec_timewarp_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_DEC_TIMEWARP success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_SET_ENC_MINMAX_RATE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_enc_minmax_rate_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_ENC_MINMAX_RATE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.handle = hdl;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_enc_minmax_rate_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_ENC_MINMAX_RATE success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_SET_ENC_RATE_MODULATION:
            {
              resp_len = sizeof ( resp_msg->u.qmi_csd_enc_rate_modulation_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_ENC_RATE_MODULATION failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.handle = hdl;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_enc_rate_modulation_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_ENC_RATE_MODULATION success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_qcelp_set_rate_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE failed, rc[0x%x]", rc );
              }
              else 
              {
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.handle = hdl;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_qcelp_set_rate_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_VOC_4GVNB_SET_RATE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_4gvnb_set_rate_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_4GVNB_SET_RATE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.handle = hdl;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_4gvnb_set_rate_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_4GVNB_SET_RATE success" );
              }
            }
            break;
            
            case CSD_VS_IOCTL_VOC_4GVWB_SET_RATE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_4gvwb_set_rate_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_4GVWB_SET_RATE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.handle = hdl;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_4gvwb_set_rate_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_4GVWB_SET_RATE success" );
              }
            }
            break;
          
            case CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_amr_set_enc_rate_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.handle = hdl;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_amr_set_enc_rate_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE success" );
              }    
            }
            break;
          
            case CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE:
            {
              resp_len = sizeof(resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.handle = hdl;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE success" );
              }
            }
            break;
          
            default:
              rc = CSD_ENOTFOUND;
              CSD_MSG(CSD_PRIO_ERROR,"Not supported cmd_param_cmd_type[0x%x]", cmd_param_cmd_type);
            break;
          }
        
          if( rc != CSD_ENOTFOUND ) 
          {
            resp_err = qmi_csi_send_resp( req_handle, msg_id, resp_msg, resp_len );
            if( resp_err != QMI_CSI_NO_ERR )
            {
              CSD_MSG( CSD_PRIO_ERROR, "qmi_csi_send_resp failed, resp_err[0x%x]", resp_err );
            }
            else
            {
              QMI_CSD_RESP_SET_BUFFER(msg_id, rc );
              CSD_MSG( CSD_PRIO_HIGH, "qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
            }
            /*Sent event to QMI thread*/
            CSD_MSG(CSD_PRIO_MEDIUM,"Sending Event to QMI thread \n");
            csd_event_signal(qmi_csd_sync_event);
            CSD_MSG(CSD_PRIO_MEDIUM,"Event has been sent QMI thread \n");
          }
          free( resp_msg );
        }
        else
        {
          CSD_MSG( CSD_PRIO_ERROR, "Resp_msg memory allocation falied" );
        }
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR,"Could not get valid list item for given token \n");
      }
    }
    break;
  
    case CSD_VS_EVENTID_DTMF_GENERATION_ENDED:
    {
      rsp = (csd_vs_event_dtmf_generation_ended_t*)param;
      ind_info.handle = hdl;
      ind_info.cmd_token = cmd_param_cmd_token;
      ind_info.direction = rsp->direction;
      msg_id = QMI_CSD_IOCTL_VS_DTMF_GENERATION_ENDED_IND_V01;
      
      CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_EVENTID_DTMF_GENERATION_ENDED, direction[%d]", ind_info.direction );
      
      resp_err = qmi_csi_send_ind( clnt_hdl, msg_id, &ind_info, sizeof(ind_info) );
      
      if( resp_err != QMI_CSI_NO_ERR )
      {
        rc = QMI_CSI_CB_INTERNAL_ERR;
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csi_send_ind failed, resp_err[0x%x]", resp_err );
      }
      else
      {
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
      }
    }
    break;

    case CSD_VS_EVENTID_RX_DTMF_DETECTED:
    {
      rsp1 = (csd_vs_event_rx_dtmf_detected_t*)param;
      ind_info1.handle = hdl;
      ind_info1.dtmf_low_freq = rsp1->low_freq;
      ind_info1.dtmf_high_freq = rsp1->high_freq;
      
      CSD_MSG( CSD_PRIO_HIGH,"CSD_VS_EVENTID_RX_DTMF_DETECTED low_freq[%d], high_freq[%d]",
               rsp1->low_freq, rsp1->high_freq );
      
      msg_id = QMI_CSD_IOCTL_VS_RX_DTMF_DETECTED_IND_V01;
      resp_err = qmi_csi_send_ind( clnt_hdl, msg_id, &ind_info1, sizeof(ind_info1) );
      
      if( resp_err != QMI_CSI_NO_ERR )
      {
        rc = QMI_CSI_CB_INTERNAL_ERR;
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csi_send_ind failed, resp_err[0x%x]", resp_err );
      }
    }
    break;
    
    case CSD_VS_EVENTID_STATE_NOTIFY:
    case CSD_VS_EVENTID_SIGNAL_ENC_BUFFER:
    case CSD_VS_EVENTID_SIGNAL_DEC_BUFFER:
      rc = CSD_EOK;
    break;

    default:
      rc = CSD_ENOTFOUND;
      CSD_MSG( CSD_PRIO_ERROR,"Invalid event_id[0x%x]", event_id );
    break;
  } 

  return rc;
}


/*=============================================================================
  FUNCTION csd_open_passive_control_voice_stream_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_OPEN_PASSIVE_CONTROL_VOICE_STREAM_REQ
  message is received by the service, sending a response message with the voice
  stream handler.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error csd_open_passive_control_voice_stream_req_handler
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
)
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
  qmi_csi_error resp_err;
  qmi_csd_open_passive_control_voice_stream_req_msg_v01 *req_ptr;
  qmi_csd_open_passive_control_voice_stream_resp_msg_v01 resp;
  char_t voice_session_name[SESSION_NAME_MAX_LEN_V01 + 1];
  uint32_t size,vs_handle;
  uint32_t csd_rc = CSD_EOK;
  csd_vs_open_t qmi_csd_svc_open_vs_info;
  //csd_cb_data_type csd_vs_cb_data;

  req_ptr = (qmi_csd_open_passive_control_voice_stream_req_msg_v01*)(req_c_struct);

  csd_vs_cb_data.clnt_info_data = clnt_info;

  qmi_csd_svc_open_vs_info.open_id = CSD_VS_OPENID_PASSIVE_CONTROL;
  qmi_csd_svc_open_vs_info.u.passive_control.callback_fn = qmi_csd_vs_callback;
  qmi_csd_svc_open_vs_info.u.passive_control.cb_data = (void* )&csd_vs_cb_data;

  size = strlcpy(voice_session_name, req_ptr->session_name, sizeof(voice_session_name));
  qmi_csd_svc_open_vs_info.u.passive_control.name = voice_session_name;
  qmi_csd_svc_open_vs_info.u.passive_control.name_size = (size+1);
  qmi_csd_svc_open_vs_info.ret_status = &csd_rc;

  CSD_MSG( CSD_PRIO_HIGH, "QMI_CSD_OPEN_PASSIVE_CONTROL_VOICE_STREAM_REQ_V01 called, stream session name[%s]", voice_session_name );   
  
  vs_handle = csd_open( CSD_OPEN_VOICE_STREAM,
                        &qmi_csd_svc_open_vs_info, sizeof(qmi_csd_svc_open_vs_info) );

  csd_vs_cb_data.handle = vs_handle;

  memset( &resp, 0, sizeof(qmi_csd_open_passive_control_voice_stream_resp_msg_v01) );

  if( vs_handle == 0 )
  {
    csd_rc = CSD_EFAILED;
    CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_OPENID_PASSIVE_CONTROL[%s] failed", voice_session_name );
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    resp.open_status_valid = TRUE;
    resp.open_status = (qmi_csd_status_v01)csd_rc;
    resp.qmi_csd_vs_passive_control_handle_valid = FALSE;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_OPENID_PASSIVE_CONTROL[%s] success, vs_handle[0x%x]", voice_session_name, vs_handle );
    resp.open_status_valid = FALSE;
    resp.qmi_csd_vs_passive_control_handle_valid = TRUE;
    resp.qmi_csd_vs_passive_control_handle = vs_handle;
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csi_send_resp failed, resp_err[0x%x]", resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    QMI_CSD_RESP_SET_BUFFER(msg_id, vs_handle );                                          
    CSD_MSG( CSD_PRIO_HIGH, "qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
  }
  return qmi_rc;
}

/*=============================================================================
  FUNCTION csd_open_full_control_voice_stream_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_OPEN_FULL_CONTROL_VOICE_STREAM_REQ
  message is received by the service.
  NOTE: This is not used by CS call, will be added later.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error csd_open_full_control_voice_stream_req_handler
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
  qmi_csi_error resp_err;
  qmi_csd_open_full_control_voice_stream_req_msg_v01 *req_ptr;
  qmi_csd_open_full_control_voice_stream_resp_msg_v01 resp;
  char_t voice_session_name[SESSION_NAME_MAX_LEN_V01 + 1];
  uint32_t size,vs_handle;
  uint32_t csd_rc = CSD_EOK;
  csd_vs_open_t qmi_csd_svc_open_vs_info;
  //csd_cb_data_type csd_vs_cb_data;
  
  req_ptr = (qmi_csd_open_full_control_voice_stream_req_msg_v01*)(req_c_struct);

  csd_vs_cb_data.clnt_info_data = clnt_info;

  qmi_csd_svc_open_vs_info.open_id = CSD_VS_OPENID_FULL_CONTROL;
  qmi_csd_svc_open_vs_info.u.full_control.callback_fn = qmi_csd_vs_callback;
  qmi_csd_svc_open_vs_info.u.full_control.cb_data = (void* )&csd_vs_cb_data;
  qmi_csd_svc_open_vs_info.u.full_control.dec_media_type = req_ptr->qmi_csd_vs_open_payload.dec_media_type;
  qmi_csd_svc_open_vs_info.u.full_control.enc_media_type = req_ptr->qmi_csd_vs_open_payload.enc_media_type;
  qmi_csd_svc_open_vs_info.u.full_control.direction = req_ptr->qmi_csd_vs_open_payload.direction;

  size = strlcpy(voice_session_name, req_ptr->qmi_csd_vs_open_payload.session_name, sizeof(voice_session_name));
  qmi_csd_svc_open_vs_info.u.full_control.name = voice_session_name;
  qmi_csd_svc_open_vs_info.u.full_control.name_size = (size+1);
  qmi_csd_svc_open_vs_info.ret_status = &csd_rc;

  CSD_MSG( CSD_PRIO_HIGH, "QMI_CSD_OPEN_FULL_CONTROL_VOICE_STREAM_REQ_V01 called, stream session name[%s], dec_media_type[0x%x], enc_media_type[0x%x], direction[%d]",  
           voice_session_name, req_ptr->qmi_csd_vs_open_payload.dec_media_type, req_ptr->qmi_csd_vs_open_payload.enc_media_type, 
           req_ptr->qmi_csd_vs_open_payload.direction );
  
  vs_handle = csd_open( CSD_OPEN_VOICE_STREAM,
                        &qmi_csd_svc_open_vs_info, sizeof(qmi_csd_svc_open_vs_info) );
                        
  csd_vs_cb_data.handle = vs_handle;

  memset( &resp, 0, sizeof(qmi_csd_open_full_control_voice_stream_resp_msg_v01) );

  if( vs_handle==0 )
  {
    csd_rc = CSD_EFAILED;
    CSD_MSG(CSD_PRIO_ERROR,"CSD_OPEN_VOICE_STREAM failed, vs_handle = 0x%x, csd_rc = 0x%x", vs_handle, csd_rc);
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    resp.open_status_valid = TRUE;
    resp.open_status = (qmi_csd_status_v01)csd_rc;
    resp.qmi_csd_vs_handle_valid = FALSE;
  }
  else
  {
    // open succeed
    CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_OPENID_FULL_CONTROL[%s] success, vs_handle = 0x%x", voice_session_name, vs_handle );
    resp.open_status_valid = FALSE;
    resp.qmi_csd_vs_handle_valid = TRUE;
    resp.qmi_csd_vs_handle = vs_handle;
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csi_send_resp failed, resp_err[0x%x]", resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc );
    CSD_MSG( CSD_PRIO_HIGH, "qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
  }
  return qmi_rc;
}

/*=============================================================================
  FUNCTION qmi_csd_ioctl_vs_cmd_req_handler
=============================================================================*/
/*!
@brief
  This function calls the CSD VS IOCTL for the QMI message recieved.

@param[in]  handle               CSD VS handle pertaining to a particular
                                 message
@param[in]  cmd_msg_id           CSD IOCTL value for the perticular 
                                 message from the client
@param[in]  clnt_info	           connection handle used by infrastructure
                                 to communicate with each client-service
                                 connection.
@param[in]  req_handle           Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.
@param[in]  msg_id               Message Id pertaining to a particular
                                 message.
@param[in]  req_c_struct         C struct with the decoded data.
@param[in]  req_c_struct_len     Length of the c struct.
@param[in]  service_cookie       Service specific data. Service cookie is
                                 registered with the infrastructure during
                                 service registration(qmi_csi_register).

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error qmi_csd_ioctl_vs_cmd_req_handler
(
  uint32_t                 handle,
  uint32_t                 cmd_msg_id,
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
  )
{
  uint32_t csd_rc = CSD_EFAILED;
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err = QMI_CSI_INTERNAL_ERR;
  uint32_t vs_hdl, resp_msg_size = 0;
  csd_cmd_token_param_t cmd_info;
  qmi_csd_vs_cmd_req_msg *qmi_csd_vs_req = (qmi_csd_vs_cmd_req_msg *)req_c_struct;
  service_context_type *context = (service_context_type*)service_cookie;
  qmi_csd_vs_cmd_resp_msg *resp_msg;
  uint32_t client_cmd_token;
  int32_t apr_rc = APR_EOK;  
  
  vs_hdl = handle;

  memset( &cmd_info, 0, sizeof(csd_cmd_token_param_t) );
  cmd_info.cmd_type = cmd_msg_id;
  cmd_info.clnt_info = clnt_info;
  cmd_info.req_handle = req_handle;
  cmd_info.msg_id = msg_id;
  cmd_info.service_cookie = context;
  cmd_info.cmd_resp_ptr = NULL;
  
  resp_msg = (qmi_csd_vs_cmd_resp_msg*)malloc(sizeof(qmi_csd_vs_cmd_resp_msg));
  
  if( resp_msg != NULL )
  {
    switch( cmd_msg_id ) 
    {
      case CSD_VS_IOCTL_SET_MEDIA_TYPE:
      {
        /*csd_vs_ioctl_set_media_type_t qmi_csd_req_info_media_type;
        qmi_csd_vs_ioctl_set_media_type_v01 *req_info_media_type = (qmi_csd_vs_ioctl_set_media_type_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_set_media_type_req.qmi_csd_vs_ioctl_set_media_type_payload));                
        
        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_set_media_type_req.qmi_csd_vs_ioctl_set_media_type_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_media_type.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_media_type.rx_media_id = req_info_media_type->rx_media_id;
        qmi_csd_req_info_media_type.tx_media_id = req_info_media_type->tx_media_id;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_MEDIA_TYPE called, rx_media_id[0x%x] tx_media_id[0x%x]", 
                 req_info_media_type->rx_media_id, req_info_media_type->tx_media_id );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_MEDIA_TYPE, &qmi_csd_req_info_media_type, sizeof(csd_vs_ioctl_set_media_type_t) );*/

        /*testing hack 
        csdlite_device_info_t temp_dev;
        csdlite_voice_config_t temp_config;
        temp_dev.bits_per_sample = CSDLITE_BPS_16;
        temp_dev.sample_rate = CSDLITE_SR_16K;
        temp_dev.tx_dev_id = 8;
        temp_dev.rx_dev_id = 10;
        temp_dev.ec_ref_dev_id = CSDLITE_INVALID_DEV;
        temp_config.cmd_token = 1;
        temp_config.dev_info = temp_dev;
        temp_config.session_id = "default modem voice\0";

        csd_rc = csdlite_voice_config(temp_config);

        temp_dev.tx_dev_id = 4;
        temp_dev.rx_dev_id = 7;
        temp_config.dev_info = temp_dev;
        csd_rc = csdlite_voice_start(temp_config);

        
        
        if( csd_rc != CSD_EOK ) 
        {

          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_MEDIA_TYPE failed, csd_rc0[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_media_type_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_media_type_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_media_type_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_media_type_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_set_media_type_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_media_type_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_media_type_resp);
        }
        */
      }
      break;

      case CSD_VS_IOCTL_SET_ENCODER_DTX_MODE:
      {
        csd_vs_ioctl_set_encoder_dtx_mode_t qmi_csd_req_info_enc_dtx_mode;
        qmi_csd_vs_ioctl_set_encoder_dtx_mode_v01 *req_info_enc_dtx_mode = (qmi_csd_vs_ioctl_set_encoder_dtx_mode_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_enc_dtx_mode_req.qmi_csd_vs_ioctl_set_encoder_dtx_mode_payload));
    
        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_enc_dtx_mode_req.qmi_csd_vs_ioctl_set_encoder_dtx_mode_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 
    
        qmi_csd_req_info_enc_dtx_mode.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_enc_dtx_mode.enable = req_info_enc_dtx_mode->enable;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_ENCODER_DTX_MODE called, enable[%d]", req_info_enc_dtx_mode->enable );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
    
        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_ENCODER_DTX_MODE, &qmi_csd_req_info_enc_dtx_mode, sizeof(csd_vs_ioctl_set_encoder_dtx_mode_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_ENCODER_DTX_MODE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_enc_dtx_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_enc_dtx_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_enc_dtx_mode_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_enc_dtx_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_enc_dtx_mode_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_enc_dtx_mode_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_enc_dtx_mode_resp);
        }
      }     
      break;

      case CSD_VS_IOCTL_SET_DEC_TIMEWARP:
      {
        csd_vs_ioctl_set_dec_timewarp_t qmi_csd_req_info_set_dec_timewarp;
        qmi_csd_vs_ioctl_set_dec_timewarp_v01 *req_info_dec_timewarp= (qmi_csd_vs_ioctl_set_dec_timewarp_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_dec_timewarp_req.qmi_csd_vs_ioctl_set_dec_timewarp_payload));                
        
        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_dec_timewarp_req.qmi_csd_vs_ioctl_set_dec_timewarp_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_info_set_dec_timewarp.cmd_token          = cmd_info.cmd_token;
        qmi_csd_req_info_set_dec_timewarp.enable_phase_match = req_info_dec_timewarp->enable_phase_match;
        qmi_csd_req_info_set_dec_timewarp.enable_time_warp   = req_info_dec_timewarp->enable_time_warp;
        qmi_csd_req_info_set_dec_timewarp.factor             = req_info_dec_timewarp->factor;
        qmi_csd_req_info_set_dec_timewarp.phase_offset       = req_info_dec_timewarp->phase_offset;
        qmi_csd_req_info_set_dec_timewarp.run_length         = req_info_dec_timewarp->run_length;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_DEC_TIMEWARP called, phase_match[0x%x] enable_time_warp[0x%x] factor[0x%x] phase_offset[0x%x] run_len[%d]", 
                 req_info_dec_timewarp->enable_phase_match, req_info_dec_timewarp->enable_time_warp, req_info_dec_timewarp->factor,
                 req_info_dec_timewarp->phase_offset, req_info_dec_timewarp->run_length );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl (vs_hdl, CSD_VS_IOCTL_SET_DEC_TIMEWARP, &qmi_csd_req_info_set_dec_timewarp, sizeof(csd_vs_ioctl_set_dec_timewarp_t));
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_DEC_TIMEWARP failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_dec_timewarp_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dec_timewarp_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dec_timewarp_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dec_timewarp_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_dec_timewarp_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_dec_timewarp_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_dec_timewarp_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_SET_ENC_MINMAX_RATE:
      {
        csd_vs_ioctl_set_enc_minmax_rate_t qmi_csd_req_info_enc_minmax_rate;
        qmi_csd_vs_ioctl_set_enc_minmax_rate_v01 *req_info_enc_minmax_rate= (qmi_csd_vs_ioctl_set_enc_minmax_rate_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_enc_minmax_rate_req.qmi_csd_vs_ioctl_set_enc_minmax_rate_payload));
        
        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_enc_minmax_rate_req.qmi_csd_vs_ioctl_set_enc_minmax_rate_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_enc_minmax_rate.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_enc_minmax_rate.max_rate  = req_info_enc_minmax_rate->max_rate;
        qmi_csd_req_info_enc_minmax_rate.min_rate  = req_info_enc_minmax_rate->min_rate;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_DEC_TIMEWARP called, max_rate[0x%x] min_rate[0x%x]", 
                 req_info_enc_minmax_rate->max_rate, req_info_enc_minmax_rate->min_rate );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_ENC_MINMAX_RATE, &qmi_csd_req_info_enc_minmax_rate, sizeof(csd_vs_ioctl_set_enc_minmax_rate_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_ENC_MINMAX_RATE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_enc_minmax_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_enc_minmax_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_enc_minmax_rate_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_enc_minmax_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_enc_minmax_rate_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_enc_minmax_rate_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_enc_minmax_rate_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_SET_ENC_RATE_MODULATION:
      {
        csd_vs_ioctl_set_enc_rate_modulation_t qmi_csd_req_info_enc_rate_mod;
        qmi_csd_vs_ioctl_set_enc_rate_mod_v01 *req_info_enc_rate_mod= (qmi_csd_vs_ioctl_set_enc_rate_mod_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_enc_rate_modulation_req.qmi_csd_vs_ioctl_set_enc_rate_mod_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_enc_rate_modulation_req.qmi_csd_vs_ioctl_set_enc_rate_mod_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_info_enc_rate_mod.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_enc_rate_mod.mode      = req_info_enc_rate_mod->mode;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_ENC_RATE_MODULATION called, mode[%d]", req_info_enc_rate_mod->mode );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_ENC_RATE_MODULATION, &qmi_csd_req_info_enc_rate_mod, sizeof(csd_vs_ioctl_set_enc_rate_modulation_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_ENC_RATE_MODULATION failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_enc_rate_modulation_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_enc_rate_modulation_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_enc_rate_modulation_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_enc_rate_modulation_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_enc_rate_modulation_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_enc_rate_modulation_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_enc_rate_modulation_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE:
      {
        csd_vs_ioctl_voc_qcelp13k_set_rate_t qmi_csd_req_info_qcelp13k_set_rate;
        qmi_csd_vs_ioctl_voc_qcelp13k_set_rate_v01 *req_info_set_rate_qcelp = (qmi_csd_vs_ioctl_voc_qcelp13k_set_rate_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_qcelp_set_rate_req.qmi_csd_vs_ioctl_voc_qcelp13k_set_rate_payload));                
        
        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_qcelp_set_rate_req.qmi_csd_vs_ioctl_voc_qcelp13k_set_rate_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_info_qcelp13k_set_rate.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_qcelp13k_set_rate.rate      = req_info_set_rate_qcelp->rate;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE called, rate[0x%x]", req_info_set_rate_qcelp->rate );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE, &qmi_csd_req_info_qcelp13k_set_rate, sizeof(csd_vs_ioctl_voc_qcelp13k_set_rate_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_qcelp_set_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_qcelp_set_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_qcelp_set_rate_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_qcelp_set_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_qcelp_set_rate_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_qcelp_set_rate_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_qcelp_set_rate_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_VOC_4GVNB_SET_RATE:
      {
        csd_vs_ioctl_voc_4gvnb_set_rate_t qmi_csd_req_info_4gvnb_set_rate;
        qmi_csd_vs_ioctl_voc_4gvnb_set_rate_v01 *req_info_set_rate_4gvnb= (qmi_csd_vs_ioctl_voc_4gvnb_set_rate_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_4gvnb_set_rate_req.qmi_csd_vs_ioctl_voc_4gvnb_set_rate_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_4gvnb_set_rate_req.qmi_csd_vs_ioctl_voc_4gvnb_set_rate_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_4gvnb_set_rate.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_4gvnb_set_rate.rate      = req_info_set_rate_4gvnb->rate;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_4GVNB_SET_RATE called, rate[0x%x]", req_info_set_rate_4gvnb->rate );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_VOC_4GVNB_SET_RATE, &qmi_csd_req_info_4gvnb_set_rate, sizeof(csd_vs_ioctl_voc_4gvnb_set_rate_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_4GVNB_SET_RATE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_4gvnb_set_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_4gvnb_set_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_4gvnb_set_rate_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_4gvnb_set_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_4gvnb_set_rate_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_4gvnb_set_rate_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_4gvnb_set_rate_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_VOC_4GVWB_SET_RATE:
      {
        csd_vs_ioctl_voc_4gvwb_set_rate_t qmi_csd_req_info_4gvwb_set_rate;
        qmi_csd_vs_ioctl_voc_4gvwb_set_rate_v01 *req_info_set_rate_4gvwb = (qmi_csd_vs_ioctl_voc_4gvwb_set_rate_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_4gvwb_set_rate_req.qmi_csd_vs_ioctl_voc_4gvwb_set_rate_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_4gvwb_set_rate_req.qmi_csd_vs_ioctl_voc_4gvwb_set_rate_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_4gvwb_set_rate.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_4gvwb_set_rate.rate      = req_info_set_rate_4gvwb->rate;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_4GVWB_SET_RATE called, rate[0x%x]", req_info_set_rate_4gvwb->rate );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_VOC_4GVWB_SET_RATE, &qmi_csd_req_info_4gvwb_set_rate, sizeof(csd_vs_ioctl_voc_4gvwb_set_rate_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_4GVWB_SET_RATE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_4gvwb_set_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_4gvwb_set_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_4gvwb_set_rate_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_4gvwb_set_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_4gvwb_set_rate_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_4gvwb_set_rate_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_4gvwb_set_rate_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE:
      {
        csd_vs_ioctl_voc_amr_set_enc_rate_t qmi_csd_req_info_amr_set_rate;
        qmi_csd_vs_ioctl_voc_amr_set_enc_rate_v01 *req_info_set_rate_amr= (qmi_csd_vs_ioctl_voc_amr_set_enc_rate_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_amr_set_enc_rate_req.qmi_csd_vs_ioctl_voc_amr_set_enc_rate_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_amr_set_enc_rate_req.qmi_csd_vs_ioctl_voc_amr_set_enc_rate_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_amr_set_rate.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_amr_set_rate.mode     = req_info_set_rate_amr->mode;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE called, mode[0x%x]", req_info_set_rate_amr->mode );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE, &qmi_csd_req_info_amr_set_rate, sizeof(csd_vs_ioctl_voc_4gvwb_set_rate_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_amr_set_enc_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_amr_set_enc_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_amr_set_enc_rate_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_amr_set_enc_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_amr_set_enc_rate_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_amr_set_enc_rate_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_amr_set_enc_rate_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE:
      {
        csd_vs_ioctl_voc_amrwb_set_enc_rate_t qmi_csd_req_info_amrwb_set_rate;
        qmi_csd_vs_ioctl_voc_amrwb_set_enc_rate_v01 *req_info_set_rate_amrwb= (qmi_csd_vs_ioctl_voc_amrwb_set_enc_rate_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_amrwb_set_enc_rate_req.qmi_csd_vs_ioctl_voc_amrwb_set_enc_rate_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_amrwb_set_enc_rate_req.qmi_csd_vs_ioctl_voc_amrwb_set_enc_rate_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_info_amrwb_set_rate.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_amrwb_set_rate.mode      = req_info_set_rate_amrwb->mode;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE called, mode[0x%x]", req_info_set_rate_amrwb->mode );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE, &qmi_csd_req_info_amrwb_set_rate, sizeof(csd_vs_ioctl_voc_4gvwb_set_rate_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_amrwb_set_enc_rate_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_SET_MUTE: 
      {
        csd_vs_ioctl_set_mute_v2_t qmi_csd_req_set_mute;
        qmi_csd_vs_ioctl_set_mute_v01 *req_info_set_mute = (qmi_csd_vs_ioctl_set_mute_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_set_mute_req.qmi_csd_vs_ioctl_set_mute_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_set_mute_req.qmi_csd_vs_ioctl_set_mute_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_set_mute.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_set_mute.direction = req_info_set_mute->direction;
        qmi_csd_req_set_mute.mute_flag = req_info_set_mute->mute_flag;
        qmi_csd_req_set_mute.ramp_duration = 0;
        
        if( qmi_csd_vs_req->u.qmi_csd_set_mute_req.ramp_duration_valid == TRUE )
        {
          qmi_csd_req_set_mute.ramp_duration = qmi_csd_vs_req->u.qmi_csd_set_mute_req.ramp_duration;
        }
        
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_MUTE called, direction[%d] mute_flag[%d] ramp_duration[%d]", 
                 req_info_set_mute->direction, req_info_set_mute->mute_flag, qmi_csd_req_set_mute.ramp_duration );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_MUTE_V2, &qmi_csd_req_set_mute, sizeof(csd_vs_ioctl_set_mute_v2_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_MUTE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_mute_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_mute_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_set_mute_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_mute_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_mute_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_SET_UI_PROPERTY: /* One or more */
      {
        csd_vs_ioctl_set_ui_property_t qmi_csd_req_info_set_ui_prop;
        csd_cmd_param_UI_property_data_t param_payload;
        qmi_csd_vs_ioctl_set_ui_property_v01 *req_info_set_ui_prop = (qmi_csd_vs_ioctl_set_ui_property_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_set_ui_prop_req.qmi_csd_vs_ioctl_set_ui_property_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_set_ui_prop_req.qmi_csd_vs_ioctl_set_ui_property_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_info_set_ui_prop.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_set_ui_prop.module_id = req_info_set_ui_prop->module_id;
        qmi_csd_req_info_set_ui_prop.param_id  = req_info_set_ui_prop->param_id;
        qmi_csd_req_info_set_ui_prop.param_size = req_info_set_ui_prop->param_data_len + (( 4 - ( req_info_set_ui_prop->param_data_len & 0x00000003)) & 0x00000003);
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_UI_PROPERTY called, module_id[0x%x] param_id[0x%x] data_len[0x%x] param_size[0x%x]", 
                 req_info_set_ui_prop->module_id, req_info_set_ui_prop->param_id, req_info_set_ui_prop->param_data_len, qmi_csd_req_info_set_ui_prop.param_size );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_memscpy( param_payload.param_data, sizeof(param_payload.param_data),
                     req_info_set_ui_prop->param_data, qmi_csd_req_info_set_ui_prop.param_size );
        qmi_csd_req_info_set_ui_prop.param_data = &param_payload.param_data;

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_UI_PROPERTY, &qmi_csd_req_info_set_ui_prop, sizeof(csd_vs_ioctl_set_ui_property_t) );
        if( csd_rc != CSD_EOK )  
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_UI_PROPERTY failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_ui_prop_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_ui_prop_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_ui_prop_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_ui_prop_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_set_ui_prop_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_ui_prop_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_ui_prop_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_GET_UI_PROPERTY:/* One at a time */
      {
        csd_vs_ioctl_get_ui_property_t qmi_csd_req_info_get_ui_prop;
        csd_cmd_param_UI_property_data_t param_payload;
        qmi_csd_vs_ioctl_get_ui_property_req_v01 *req_info_get_ui_prop = (qmi_csd_vs_ioctl_get_ui_property_req_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_set_ui_prop_req.qmi_csd_vs_ioctl_set_ui_property_payload));                

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_get_ui_prop_req.qmi_csd_vs_ioctl_get_ui_property_req_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_get_ui_prop.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_get_ui_prop.module_id = req_info_get_ui_prop->module_id;
        qmi_csd_req_info_get_ui_prop.param_id  = req_info_get_ui_prop->param_id;
        qmi_csd_req_info_get_ui_prop.param_data = &param_payload;
        qmi_csd_req_info_get_ui_prop.param_size = sizeof(csd_cmd_param_UI_property_data_t);
        
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_GET_UI_PROPERTY called, module_id[0x%x] param_id[0x%x] param_size[0x%x]", 
                 req_info_get_ui_prop->module_id, req_info_get_ui_prop->param_id, qmi_csd_req_info_get_ui_prop.param_size );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_GET_UI_PROPERTY, &qmi_csd_req_info_get_ui_prop, sizeof(csd_vs_ioctl_get_ui_property_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_GET_UI_PROPERTY failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_get_ui_prop_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_get_ui_prop_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_get_ui_prop_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_get_ui_prop_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_get_ui_prop_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_get_ui_prop_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_START_RECORD:
      {
        csd_vs_ioctl_start_record_v2_t qmi_csd_req_start_record;
        qmi_csd_ioctl_vs_cmd_start_record_req_msg_v01 *req_info_start_record = (qmi_csd_ioctl_vs_cmd_start_record_req_msg_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_start_record_req));           

        cmd_info.cmd_token = req_info_start_record->cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_start_record.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_start_record.rx_tap_point = req_info_start_record->rx_tap_point;
        qmi_csd_req_start_record.tx_tap_point = req_info_start_record->tx_tap_point;
        qmi_csd_req_start_record.dev_id = 0xFFFFFFFF;
        qmi_csd_req_start_record.mode = 0;
        
        if( ( req_info_start_record->dev_id_valid == TRUE) && ( req_info_start_record->mode_valid == TRUE) )
        {
          qmi_csd_req_start_record.dev_id = req_info_start_record->dev_id;
          qmi_csd_req_start_record.mode = req_info_start_record->mode;  
        }
        
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_START_RECORD called, rx_tap_point[0x%x] tx_tap_point[0x%x] dev_id[0x%x] mode[0x%x]", 
                 req_info_start_record->rx_tap_point, req_info_start_record->tx_tap_point, qmi_csd_req_start_record.dev_id, qmi_csd_req_start_record.mode );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_START_RECORD_V2, &qmi_csd_req_start_record, sizeof(csd_vs_ioctl_start_record_v2_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_START_RECORD failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_start_record_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_start_record_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_start_record_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_start_record_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_start_record_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_start_record_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_start_record_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_STOP_RECORD:
      {
        uint32_t qmi_csd_req_stop_record;

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_stop_record_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_stop_record = cmd_info.cmd_token;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_STOP_RECORD called" );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_STOP_RECORD, &qmi_csd_req_stop_record, sizeof(uint32_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_STOP_RECORD failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_stop_record_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_stop_record_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_stop_record_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_stop_record_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_stop_record_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_stop_record_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_stop_record_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_START_PLAYBACK:
      {
        csd_vs_ioctl_start_playback_v2_t qmi_csd_req_start_playback;
        qmi_csd_ioctl_vs_cmd_start_playback_req_msg_v01 *req_info_start_playback = (qmi_csd_ioctl_vs_cmd_start_playback_req_msg_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_start_playback_req));           
        
        cmd_info.cmd_token = req_info_start_playback->cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info ); 

        qmi_csd_req_start_playback.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_start_playback.dev_id = req_info_start_playback->dev_id;
        
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_START_PLAYBACK called, dev_id[0x%x]", req_info_start_playback->dev_id );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
 
        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_START_PLAYBACK_V2, &qmi_csd_req_start_playback, sizeof(csd_vs_ioctl_start_playback_v2_t) );
       
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_START_PLAYBACK failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_start_playback_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_start_playback_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_start_playback_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_start_playback_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_start_playback_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_start_playback_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_start_playback_resp);
        }
      }
      break;
      
      case CSD_VS_IOCTL_STOP_PLAYBACK:
      {
        uint32_t qmi_csd_req_stop_playback;
  
        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_stop_playback_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_stop_playback = cmd_info.cmd_token;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_STOP_PLAYBACK called" );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_STOP_PLAYBACK, &qmi_csd_req_stop_playback, sizeof(uint32_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_STOP_PLAYBACK failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_stop_playback_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_stop_playback_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_stop_playback_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_stop_playback_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_stop_playback_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_stop_playback_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_stop_playback_resp);
        }
      }
      break;
      
      case CSD_VS_IOCTL_SET_DTMF_GENERATION:
      {
        csd_vs_ioctl_set_dtmf_generation_t qmi_csd_req_dtmf_gen;
        qmi_csd_vs_ioctl_set_dtmf_generation_v01 *req_info_dtmf_gen = (qmi_csd_vs_ioctl_set_dtmf_generation_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_dtmf_gen_req.qmi_csd_vs_ioctl_set_dtmf_generation_payload));           

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_dtmf_gen_req.qmi_csd_vs_ioctl_set_dtmf_generation_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_dtmf_gen.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_dtmf_gen.direction = req_info_dtmf_gen->direction;
        qmi_csd_req_dtmf_gen.duration  = req_info_dtmf_gen->duration;
        qmi_csd_req_dtmf_gen.gain      = req_info_dtmf_gen->gain;
        qmi_csd_req_dtmf_gen.mix_flag  = req_info_dtmf_gen->mix_flag;
        qmi_csd_req_dtmf_gen.tone_1    = req_info_dtmf_gen->tone_1;
        qmi_csd_req_dtmf_gen.tone_2    = req_info_dtmf_gen->tone_2;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_DTMF_GENERATION called, direction[%d] duration[%d] gain[0x%x] mix_flag[0x%x] tone1[%d] tone2[%d]",
                 req_info_dtmf_gen->direction, req_info_dtmf_gen->duration, req_info_dtmf_gen->gain, req_info_dtmf_gen->mix_flag,
                 req_info_dtmf_gen->tone_1, req_info_dtmf_gen->tone_2 );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_DTMF_GENERATION, &qmi_csd_req_dtmf_gen, sizeof(csd_vs_ioctl_set_dtmf_generation_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_DTMF_GENERATION failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_dtmf_gen_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dtmf_gen_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dtmf_gen_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dtmf_gen_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_dtmf_gen_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_dtmf_gen_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_dtmf_gen_resp);
        }
      }
      break;

      case CSD_VS_IOCTL_SET_RX_DTMF_DETECTION:
      {
        csd_vs_ioctl_set_rx_dtmf_detection_t qmi_csd_req_rx_dtmf_dect;
        qmi_csd_vs_ioctl_set_rx_dtmf_detection_v01 *req_info_rx_dtmf_dect = (qmi_csd_vs_ioctl_set_rx_dtmf_detection_v01 *)(&(qmi_csd_vs_req->u.qmi_csd_rx_dtmf_dect_req.qmi_csd_vs_ioctl_set_rx_dtmf_detection_payload));           

        cmd_info.cmd_token = qmi_csd_vs_req->u.qmi_csd_rx_dtmf_dect_req.qmi_csd_vs_ioctl_set_rx_dtmf_detection_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_rx_dtmf_dect.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_rx_dtmf_dect.enable    = req_info_rx_dtmf_dect->enable;
        CSD_MSG( CSD_PRIO_HIGH, "CSD_VS_IOCTL_SET_RX_DTMF_DETECTION called, enable[%d]", req_info_rx_dtmf_dect->enable );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vs_hdl, CSD_VS_IOCTL_SET_RX_DTMF_DETECTION, &qmi_csd_req_rx_dtmf_dect, sizeof(csd_vs_ioctl_set_rx_dtmf_detection_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VS_IOCTL_SET_RX_DTMF_DETECTION failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_rx_dtmf_dect_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_rx_dtmf_dect_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_rx_dtmf_dect_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_rx_dtmf_dect_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
          resp_msg->u.qmi_csd_rx_dtmf_dect_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_rx_dtmf_dect_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_rx_dtmf_dect_resp);
        }
      }
      break;
    }
    
    /* only when error happens, send response message here.
       For success case response will be sent in the event callbacks. */
    if( csd_rc != CSD_EOK ) 
    {
      resp_err = qmi_csi_send_resp( req_handle, msg_id, resp_msg, resp_msg_size );
      if( resp_err != QMI_CSI_NO_ERR )
      {
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csi_send_resp failed, resp_err[0x%x]", resp_err );
      }
      else
      {
        QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc );
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
      }
    }
    else
    {
      qmi_rc = QMI_CSI_CB_NO_ERR;
      /*Wait for the Event from the voice thread*/
      apr_rc = csd_event_wait( qmi_csd_sync_event );
      if( apr_rc != APR_EOK )
      {
          CSD_MSG( CSD_PRIO_ERROR,"event wait failed, rc = %x", apr_rc );
      }
      else
      {
        CSD_MSG(CSD_PRIO_MEDIUM,"Event received from the voc thread");
      }
    }
    free( resp_msg );
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR, "Resp_msg allocation Failed" );
  }
  
  return qmi_rc;
}
