/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S _ V M . C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file for voice context.


Copyright (c) 2011-2013, 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services_vm.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/15/16    rk     Abstracting OSAL layer
09/01/14    rp     replaced the qmi debug buffer with uniform csd debug buffer
07/03/13    vk     Changes to remove compiler warnings
05/24/13    rp     Added session name logging 
                   Added support for asynchronized qmi csd clients apis
01/23/12    jk     Added support for CSD_VM_IOCTL_PAUSE_VOICE API
06/26/12    jk     Added support for not calling callback when client is disconnected 
04/19/12    rp     Added support for voice standby mode
02/22/12    jp/rp  Removed the unwanted debug message
09/12/11    akj     Created file (initial version)

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "csd_qmi_services_vm.h"
#include "csd_vm_ioctl.h"
#include "csd_vm.h"
#include "csd_debug_info.h"
#include "apr_errcodes.h"
#include "csd_os_dependencies.h"

extern csd_event_t qmi_csd_sync_event;
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/*=============================================================================
                    QMI CSD Voice Manager Callback Functions
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_vm_callback
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

uint32_t qmi_csd_vm_callback(uint32_t cmd_token, uint32_t event_id,
                         void* param, uint32_t param_size, void* cb_data_info)
{
  uint32_t rc, cmd_param_cmd_token, cmd_param_cmd_type, hdl;
  csd_cmd_token_param_t cmd_param_info;
  //qmi_client_handle clnt_hdl;

  qmi_req_handle req_handle;
  int32_t msg_id, resp_len = 0;
  qmi_csi_error resp_err;

  qmi_csd_vm_cmd_resp_msg *resp_msg;

  csd_cb_data_type *cb_data_ptr = (csd_cb_data_type*)cb_data_info;
  hdl = cb_data_ptr->handle;
  //clnt_hdl = (cb_data_ptr->clnt_info_data)->clnt;

  //cmd_param_info = (csd_cmd_token_param_t*)cmd_token;

  cmd_param_info.cmd_token = cmd_token;
  rc = qmi_csd_client_info_get_list( &cmd_param_info);
  if (CSD_EOK == rc)
  {
    cmd_param_cmd_type = cmd_param_info.cmd_type;
    cmd_param_cmd_token = cmd_param_info.cmd_token;

    req_handle = cmd_param_info.req_handle;
    msg_id = cmd_param_info.msg_id;
    
    resp_msg = (qmi_csd_vm_cmd_resp_msg*)malloc(sizeof(qmi_csd_vm_cmd_resp_msg));
    
    if( resp_msg != NULL )
    {
      memset( resp_msg, 0, sizeof(*resp_msg) );
      switch( event_id ) 
      {
        /* cmd response */
        case CSD_VM_EVENTID_COMMAND_RSP:
        {
          rc = ((csd_vm_event_command_rsp_t*)param)->status_id;
    
          switch( cmd_param_cmd_type ) 
          {
            case CSD_VM_IOCTL_ATTACH_STREAM:
              resp_len = sizeof(resp_msg->u.qmi_csd_attach_stream_resp);
           
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_attach_stream_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_attach_stream_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_attach_stream_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_attach_stream_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_attach_stream_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_attach_stream_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_ATTACH_STREAM failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_attach_stream_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_attach_stream_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_attach_stream_resp.handle = hdl;
                resp_msg->u.qmi_csd_attach_stream_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_attach_stream_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_ATTACH_STREAM success" );
              }
            break;
      
            case CSD_VM_IOCTL_DETACH_STREAM:
              resp_len = sizeof(resp_msg->u.qmi_csd_detach_stream_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_detach_stream_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_detach_stream_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_detach_stream_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_detach_stream_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_detach_stream_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_detach_stream_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_DETACH_STREAM failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_detach_stream_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_detach_stream_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_detach_stream_resp.handle = hdl;
                resp_msg->u.qmi_csd_detach_stream_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_detach_stream_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_DETACH_STREAM success" );
              }
            break;
      
            case CSD_VM_IOCTL_ATTACH_CONTEXT:
              resp_len = sizeof (resp_msg->u.qmi_csd_attach_context_resp);
          
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_attach_context_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_attach_context_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_attach_context_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_attach_context_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_attach_context_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_attach_context_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_ATTACH_CONTEXT failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_attach_context_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_attach_context_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_attach_context_resp.handle = hdl;
                resp_msg->u.qmi_csd_attach_context_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_attach_context_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_ATTACH_CONTEXT success" );
              }   
            break;
      
            case CSD_VM_IOCTL_DETACH_CONTEXT:
              resp_len = sizeof (resp_msg->u.qmi_csd_detach_context_resp);
              
              if( rc != CSD_EOK ) 
              {
                rc = CSD_EFAILED;
                resp_msg->u.qmi_csd_detach_context_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_detach_context_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_detach_context_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_detach_context_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_detach_context_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_detach_context_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_DETACH_CONTEXT failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_detach_context_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_detach_context_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_detach_context_resp.handle = hdl;
                resp_msg->u.qmi_csd_detach_context_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_detach_context_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_DETACH_CONTEXT success" );
              }
            break;
      
            case CSD_VM_IOCTL_START_VOICE:
              resp_len = sizeof (resp_msg->u.qmi_csd_start_voice_resp);
              
              if( rc != CSD_EOK ) 
              {
                rc = CSD_EFAILED;
                resp_msg->u.qmi_csd_start_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_start_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_start_voice_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_start_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_start_voice_resp.handle_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_START_VOICE failed, rc[0x%x]",rc);
              }
              else 
              { // success case
                resp_msg->u.qmi_csd_start_voice_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_start_voice_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_start_voice_resp.handle = hdl;
                resp_msg->u.qmi_csd_start_voice_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_start_voice_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_START_VOICE success" );
              }
            break;
              
            case CSD_VM_IOCTL_STANDBY_VOICE:
              resp_len = sizeof (resp_msg->u.qmi_csd_standby_voice_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_standby_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_standby_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_standby_voice_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_standby_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_standby_voice_resp.handle_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_STANDBY_VOICE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_standby_voice_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_standby_voice_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_standby_voice_resp.handle = hdl;
                resp_msg->u.qmi_csd_standby_voice_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_standby_voice_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_STANDBY_VOICE success" );
              }
            break;
             
            case CSD_VM_IOCTL_PAUSE_VOICE:
              resp_len = sizeof (resp_msg->u.qmi_csd_pause_voice_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_pause_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_pause_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_pause_voice_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_pause_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_pause_voice_resp.handle_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_PAUSE_VOICE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_pause_voice_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_pause_voice_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_pause_voice_resp.handle = hdl;
                resp_msg->u.qmi_csd_pause_voice_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_pause_voice_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_PAUSE_VOICE success" );
              }
            break;
             
            case CSD_VM_IOCTL_STOP_VOICE:
              resp_len = sizeof (resp_msg->u.qmi_csd_stop_voice_resp);
              
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_stop_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_stop_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_stop_voice_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_stop_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_stop_voice_resp.handle_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_STOP_VOICE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_stop_voice_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_stop_voice_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_stop_voice_resp.handle = hdl;
                resp_msg->u.qmi_csd_stop_voice_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_stop_voice_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_STOP_VOICE success" );
              }
            break;
      
            case CSD_VM_IOCTL_SET_TTY_MODE:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_tty_mode_resp);
              
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_tty_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_tty_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_tty_mode_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_tty_mode_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_TTY_MODE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_tty_mode_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_tty_mode_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_tty_mode_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_tty_mode_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_TTY_MODE success" );
              }
            break;
      
            case CSD_VM_IOCTL_SET_WIDEVOICE:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_widevoice_resp);
              
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_widevoice_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_widevoice_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_widevoice_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_widevoice_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_widevoice_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_widevoice_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_WIDEVOICE failed, rc[0x%x]", rc );
              }
              else 
              {
                resp_msg->u.qmi_csd_set_widevoice_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_widevoice_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_widevoice_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_widevoice_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_widevoice_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_WIDEVOICE success" );
              }
            break;
      
            case CSD_VM_IOCTL_SET_NETWORK:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_network_resp);
              
              if( rc != CSD_EOK )
              {
                resp_msg->u.qmi_csd_set_network_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_network_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_network_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_network_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_network_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_network_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_NETWORK failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_network_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_network_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_network_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_network_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_network_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_NETWORK success" );
              } 
            break;
          
            case CSD_VM_IOCTL_SET_VOICE_TIMING:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_voice_timing_resp);
              
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_voice_timing_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_voice_timing_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_voice_timing_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_voice_timing_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_voice_timing_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_voice_timing_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_VOICE_TIMING failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_voice_timing_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_voice_timing_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_voice_timing_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_voice_timing_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_voice_timing_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_VOICE_TIMING success" );
              }
            break;
          
            case CSD_VM_IOCTL_SET_HDVOICE_MODE:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_hdvoice_mode_resp);
              
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_HDVOICE_MODE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_hdvoice_mode_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: CSD_VM_IOCTL_SET_HDVOICE_MODE success" );
              }
            break;
          
            default:
              CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: Not supported cmd_param_cmd_type[0x%x]", cmd_param_cmd_type );
            break;
          } // switch cmd_type
          break;

          default:
            rc = CSD_ENOTFOUND;
          break;
        }
      } // switch for even_id
      
      if( rc != CSD_ENOTFOUND )
      {
        resp_err = qmi_csi_send_resp( req_handle, msg_id, resp_msg, resp_len );
        if( resp_err != QMI_CSI_NO_ERR )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: qmi_csi_send_resp failed, rc[0x%x]", resp_err );
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vm_callback: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
          QMI_CSD_RESP_SET_BUFFER(msg_id, rc );
        }
          /*Sent event to QMI thread*/
          CSD_MSG(CSD_PRIO_MEDIUM,"qmi_csd_vm_callback: Sending Event to QMI thread \n");
          csd_event_signal(qmi_csd_sync_event);
          CSD_MSG(CSD_PRIO_MEDIUM,"qmi_csd_vm_callback: Event has been sent QMI thread \n");
      }
      else
      {
         CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: Invalid token not sent callback to client" );
      }
      free(resp_msg);
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vm_callback: Resp_msg memory allocation failed" );
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"qmi_csd_vm_callback: could not get valid list item for given token");
  }
   return rc;
}


/*=============================================================================
  FUNCTION csd_open_voice_manager_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_OPEN_VOICE_MANAGER_REQ
  message is received by the service, sending a response message with the voice
  manager handler.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error csd_open_voice_manager_req_handler
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
  qmi_csd_vm_open_type_v01 *req_ptr;
  qmi_csd_open_voice_manager_resp_msg_v01 resp;

  csd_vm_open_t qmi_csd_svc_open_vm_info;
  csd_cb_data_type csd_vm_cb_data;
  char_t voice_session_name[SESSION_NAME_MAX_LEN_V01 + 1];
  uint32_t size, vm_handle;
  uint32_t csd_rc = CSD_EOK;

  req_ptr = &(((qmi_csd_open_voice_manager_req_msg_v01*)(req_c_struct))->qmi_csd_vm_open_payload);
  csd_vm_cb_data.clnt_info_data = clnt_info;

  qmi_csd_svc_open_vm_info.open_id = CSD_VM_OPENID_PASSIVE_CONTROL;
  qmi_csd_svc_open_vm_info.u.passive_control.callback_fn = qmi_csd_vm_callback;
  qmi_csd_svc_open_vm_info.u.passive_control.cb_data = (void* )&csd_vm_cb_data;

  size = strlcpy(voice_session_name, req_ptr->session_name, sizeof(voice_session_name));
  qmi_csd_svc_open_vm_info.u.passive_control.name = voice_session_name;
  qmi_csd_svc_open_vm_info.u.passive_control.name_size = (size+1);
  qmi_csd_svc_open_vm_info.ret_status = &csd_rc;

  CSD_MSG( CSD_PRIO_HIGH, "csd_open_voice_manager_req_handler: QMI_CSD_OPEN_VOICE_MANAGER_REQ_V01 called" );  
  
  vm_handle = csd_open( CSD_OPEN_VOICE_MANAGER,
                        &qmi_csd_svc_open_vm_info, sizeof(qmi_csd_svc_open_vm_info) );

  csd_vm_cb_data.handle = vm_handle;

  memset( &resp, 0, sizeof(qmi_csd_open_voice_manager_resp_msg_v01) );

  if( vm_handle == 0 ) 
  {
    csd_rc = CSD_EFAILED;
    CSD_MSG( CSD_PRIO_ERROR, "csd_open_voice_manager_req_handler: CSD_OPEN_VOICE_MANAGER failed" );
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    resp.open_status_valid = TRUE;
    resp.open_status = (qmi_csd_status_v01)csd_rc;
    resp.qmi_csd_vm_handle_valid = FALSE;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_open_voice_manager_req_handler: CSD_OPEN_VOICE_MANAGER success, vm_handle[0x%x]", vm_handle );
    resp.open_status_valid = FALSE;
    resp.qmi_csd_vm_handle_valid = TRUE;
    resp.qmi_csd_vm_handle = vm_handle;
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_open_voice_manager_req_handler: qmi_csi_send_resp failed, rc[0x%x]", resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_open_voice_manager_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
    QMI_CSD_RESP_SET_BUFFER(msg_id, vm_handle );   
  }
  return qmi_rc;
}

/*=============================================================================
  FUNCTION qmi_csd_ioctl_vm_cmd_req_handler
=============================================================================*/
/*!
@brief
  This function calls the CSD VM IOCTL for the QMI message recieved.

@param[in]  handle               CSD VM handle pertaining to a particular
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
qmi_csi_cb_error qmi_csd_ioctl_vm_cmd_req_handler
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
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  uint32_t csd_rc = CSD_EFAILED;
  qmi_csi_error resp_err = QMI_CSI_INTERNAL_ERR;
  uint32_t vm_hdl, resp_msg_size = 0;
  csd_cmd_token_param_t cmd_info;
  qmi_csd_vm_cmd_req_msg *qmi_csd_vm_req = (qmi_csd_vm_cmd_req_msg*)req_c_struct;
  service_context_type *context = (service_context_type*)service_cookie;
  qmi_csd_vm_cmd_resp_msg *resp_msg;
  uint32_t client_cmd_token;
  int32_t apr_rc = APR_EOK;
 
  vm_hdl = handle;
  resp_msg = (qmi_csd_vm_cmd_resp_msg*)malloc(sizeof(qmi_csd_vm_cmd_resp_msg));

  memset( &cmd_info, 0, sizeof ( csd_cmd_token_param_t ) );
  cmd_info.cmd_type = cmd_msg_id;
  cmd_info.clnt_info = clnt_info;
  cmd_info.req_handle = req_handle;
  cmd_info.msg_id = msg_id;
  cmd_info.service_cookie = context;
  cmd_info.cmd_resp_ptr = (uint32_t)resp_msg;
  if( resp_msg != NULL )
  {
    switch( cmd_msg_id ) 
    {
      case CSD_VM_IOCTL_ATTACH_STREAM: /* need to get the proper cvs handle */
      {
        csd_vm_ioctl_attach_stream_t qmi_csd_req_info_attach_stream;
        qmi_csd_vm_ioctl_attach_stream_v01 *req_info_attach_stream = (qmi_csd_vm_ioctl_attach_stream_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_attach_context_req.qmi_csd_vm_ioctl_attach_context_payload));               
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_attach_context_req.qmi_csd_vm_ioctl_attach_context_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 
        
        qmi_csd_req_info_attach_stream.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_attach_stream.stream_handle = req_info_attach_stream->stream_handle;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_ATTACH_STREAM called, stream_handle[0x%x]", req_info_attach_stream->stream_handle );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_ATTACH_STREAM, &qmi_csd_req_info_attach_stream, sizeof(csd_vm_ioctl_attach_stream_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_ATTACH_STREAM failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_attach_stream_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_attach_stream_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_attach_stream_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_attach_stream_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_attach_stream_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_attach_stream_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_attach_stream_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_DETACH_STREAM: /* need to get the proper cvs handle */
      {
        csd_vm_ioctl_detach_stream_t qmi_csd_req_info_detach_stream;
        qmi_csd_vm_ioctl_detach_stream_v01 *req_info_detach_stream = (qmi_csd_vm_ioctl_detach_stream_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_detach_context_req.qmi_csd_vm_ioctl_detach_context_payload));                
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_detach_context_req.qmi_csd_vm_ioctl_detach_context_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 
        
        qmi_csd_req_info_detach_stream.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_detach_stream.stream_handle = req_info_detach_stream->stream_handle;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_DETACH_STREAM called, stream_handle[0x%x]", req_info_detach_stream->stream_handle );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_DETACH_STREAM, &qmi_csd_req_info_detach_stream, sizeof(csd_vm_ioctl_detach_stream_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_ATTACH_STREAM failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_detach_stream_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_detach_stream_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_detach_stream_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_detach_stream_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_detach_stream_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_detach_stream_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_detach_stream_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_ATTACH_CONTEXT:
      {
        csd_vm_ioctl_attach_context_t qmi_csd_req_info_attach_context;
        qmi_csd_vm_ioctl_attach_context_v01 *req_info_attach_context = (qmi_csd_vm_ioctl_attach_context_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_attach_context_req.qmi_csd_vm_ioctl_attach_context_payload));                
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_attach_context_req.qmi_csd_vm_ioctl_attach_context_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_attach_context.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_attach_context.context_handle = req_info_attach_context->context_handle;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_ATTACH_CONTEXT called, context_handle[0x%x]", req_info_attach_context->context_handle );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_ATTACH_CONTEXT, &qmi_csd_req_info_attach_context, sizeof(csd_vm_ioctl_attach_context_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_ATTACH_CONTEXT failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_attach_context_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_attach_context_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_attach_context_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_attach_context_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_attach_context_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_attach_context_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_attach_context_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_DETACH_CONTEXT:
      {
        csd_vm_ioctl_detach_context_t qmi_csd_req_info_detach_context;
        qmi_csd_vm_ioctl_detach_context_v01 *req_info_detach_context = (qmi_csd_vm_ioctl_detach_context_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_detach_context_req.qmi_csd_vm_ioctl_detach_context_payload));                
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_detach_context_req.qmi_csd_vm_ioctl_detach_context_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_detach_context.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_detach_context.context_handle = req_info_detach_context->context_handle;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_DETACH_CONTEXT called, context_handle[0x%x]", req_info_detach_context->context_handle );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_DETACH_CONTEXT, &qmi_csd_req_info_detach_context, sizeof(csd_vm_ioctl_detach_context_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_DETACH_CONTEXT failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_detach_context_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_detach_context_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_detach_context_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_detach_context_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_detach_context_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_detach_context_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_detach_context_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_SET_NETWORK:
      {
        csd_vm_ioctl_set_network_t qmi_csd_req_info_set_network;
        qmi_csd_vm_ioctl_set_network_v01 *req_info_set_network = (qmi_csd_vm_ioctl_set_network_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_set_network_req.qmi_csd_vm_ioctl_set_network_payload));   
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_set_network_req.qmi_csd_vm_ioctl_set_network_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_set_network.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_set_network.network_id= req_info_set_network->network_id;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_NETWORK called, network_id[%d]", req_info_set_network->network_id );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_SET_NETWORK, &qmi_csd_req_info_set_network, sizeof(csd_vm_ioctl_set_network_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_NETWORK failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_network_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_network_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_network_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_network_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_set_network_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_network_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_network_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_SET_VOICE_TIMING:
      {
        csd_vm_ioctl_set_voice_timing_t qmi_csd_req_info_set_timing;
        qmi_csd_vm_ioctl_set_voice_timing_v01 *req_info_set_timing = (qmi_csd_vm_ioctl_set_voice_timing_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_set_voice_timing_req.qmi_csd_vm_ioctl_set_voice_timing_payload));   
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_set_voice_timing_req.qmi_csd_vm_ioctl_set_voice_timing_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_set_timing.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_set_timing.dec_offset = req_info_set_timing->dec_offset;
        qmi_csd_req_info_set_timing.dec_req_offset = req_info_set_timing->dec_req_offset;
        qmi_csd_req_info_set_timing.enc_offset = req_info_set_timing->enc_offset;
        qmi_csd_req_info_set_timing.mode = req_info_set_timing->mode;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_VOICE_TIMING called, dec_offset[%d] dec_req_offset[%d] enc_offset[%d] mode[%d]",
                 req_info_set_timing->dec_offset, req_info_set_timing->dec_req_offset, req_info_set_timing->enc_offset, req_info_set_timing->mode );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
                 
        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_SET_VOICE_TIMING, &qmi_csd_req_info_set_timing, sizeof(csd_vm_ioctl_set_voice_timing_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_VOICE_TIMING failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_voice_timing_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_voice_timing_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_voice_timing_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_voice_timing_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_set_voice_timing_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_voice_timing_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_voice_timing_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_SET_TTY_MODE:
      {
        csd_vm_ioctl_set_tty_mode_t qmi_csd_req_info_set_tty_mode;
        qmi_csd_vm_ioctl_set_tty_mode_v01 *req_info_set_tty_mode = (qmi_csd_vm_ioctl_set_tty_mode_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_set_tty_mode_req.qmi_csd_vm_ioctl_set_tty_mode_payload));   
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_set_tty_mode_req.qmi_csd_vm_ioctl_set_tty_mode_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_set_tty_mode.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_set_tty_mode.mode = req_info_set_tty_mode->mode;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_TTY_MODE called, mode[%d]", req_info_set_tty_mode->mode );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_SET_TTY_MODE, &qmi_csd_req_info_set_tty_mode, sizeof(csd_vm_ioctl_set_tty_mode_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_TTY_MODE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_tty_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_tty_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_set_tty_mode_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_tty_mode_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_tty_mode_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_SET_WIDEVOICE:
      {
        csd_vm_ioctl_set_widevoice_t qmi_csd_req_info_set_widevoice;
        qmi_csd_vm_ioctl_set_widevoice_v01 *req_info_set_widevoice = (qmi_csd_vm_ioctl_set_widevoice_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_set_widevoice_req.qmi_csd_vm_ioctl_set_widevoice_payload));   
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_set_widevoice_req.qmi_csd_vm_ioctl_set_widevoice_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_set_widevoice.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_set_widevoice.enable = req_info_set_widevoice->enable;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_WIDEVOICE called, enable[%d]", req_info_set_widevoice->enable );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_SET_WIDEVOICE, &qmi_csd_req_info_set_widevoice, sizeof(csd_vm_ioctl_set_widevoice_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_WIDEVOICE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_tty_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_tty_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_tty_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_set_tty_mode_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_tty_mode_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_tty_mode_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_START_VOICE:
      {
        uint32_t qmi_csd_start_voice_arg;
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_start_voice_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_start_voice_arg = cmd_info.cmd_token;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_START_VOICE called" );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_START_VOICE, &qmi_csd_start_voice_arg, sizeof(uint32_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_START_VOICE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_start_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_start_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_start_voice_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_start_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_start_voice_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_start_voice_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_start_voice_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_STANDBY_VOICE:
      {
        uint32_t qmi_csd_standby_voice_arg;

        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_standby_voice_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_standby_voice_arg = cmd_info.cmd_token;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_STANDBY_VOICE called" );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_STANDBY_VOICE, &qmi_csd_standby_voice_arg, sizeof(qmi_csd_standby_voice_arg) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_STANDBY_VOICE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_standby_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_standby_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_standby_voice_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_standby_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_standby_voice_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_standby_voice_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_standby_voice_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_PAUSE_VOICE:
      {
        uint32_t qmi_csd_pause_voice_arg;
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_pause_voice_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_pause_voice_arg = cmd_info.cmd_token;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_PAUSE_VOICE called, csd_cmd_token[0x%x]", cmd_info.cmd_token );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_PAUSE_VOICE, &qmi_csd_pause_voice_arg, sizeof(qmi_csd_pause_voice_arg) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_PAUSE_VOICE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_pause_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_pause_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_pause_voice_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_pause_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_pause_voice_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_pause_voice_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_pause_voice_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_STOP_VOICE:
      {
        uint32_t qmi_csd_stop_voice_arg;

        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_stop_voice_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_STOP_VOICE called" );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        qmi_csd_stop_voice_arg = cmd_info.cmd_token;

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_STOP_VOICE, &qmi_csd_stop_voice_arg, sizeof(uint32_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_STOP_VOICE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_stop_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_stop_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_stop_voice_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_stop_voice_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_stop_voice_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_stop_voice_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_stop_voice_resp);
        }
      }
      break;
      
      case CSD_VM_IOCTL_SET_HDVOICE_MODE:
      {
        csd_vm_ioctl_set_hdvoice_mode_t qmi_csd_req_info_set_hdvoice_mode;
        qmi_csd_vm_ioctl_set_hdvoice_mode_v01 *req_info_set_hdvoice_mode = (qmi_csd_vm_ioctl_set_hdvoice_mode_v01 *)(&(qmi_csd_vm_req->u.qmi_csd_set_hdvoice_mode_req.qmi_csd_vm_ioctl_set_hdvoice_mode_payload));   
        
        cmd_info.cmd_token = qmi_csd_vm_req->u.qmi_csd_set_hdvoice_mode_req.qmi_csd_vm_ioctl_set_hdvoice_mode_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info ); 

        qmi_csd_req_info_set_hdvoice_mode.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_set_hdvoice_mode.mode = req_info_set_hdvoice_mode->mode;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_HDVOICE_MODE called, mode[%d]", req_info_set_hdvoice_mode->mode );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vm_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vm_hdl, CSD_VM_IOCTL_SET_HDVOICE_MODE, &qmi_csd_req_info_set_hdvoice_mode, sizeof(csd_vm_ioctl_set_hdvoice_mode_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: CSD_VM_IOCTL_SET_HDVOICE_MODE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_hdvoice_mode_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_hdvoice_mode_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_hdvoice_mode_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_hdvoice_mode_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_set_hdvoice_mode_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_hdvoice_mode_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_hdvoice_mode_resp);
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
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vm_cmd_req_handler: qmi_csi_send_resp failed, resp_err[0x%x]", resp_err);      
      }
      else
      {
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vm_cmd_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
        QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc ); 
      }
    }
    else
    {
      qmi_rc = QMI_CSI_CB_NO_ERR;
      /*Wait for the Event from the voice thread*/
      apr_rc = csd_event_wait( qmi_csd_sync_event );
      if( apr_rc != APR_EOK )
      {
        CSD_MSG( CSD_PRIO_ERROR,"qmi_csd_ioctl_vm_cmd_req_handler: event wait failed, rc[%d]", apr_rc );
      }
    }
    free(resp_msg);
  }  
  return qmi_rc;
}
