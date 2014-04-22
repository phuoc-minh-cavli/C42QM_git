/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S _ V C . C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file for voice context.


Copyright (c) 2011, 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services_vc.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/15/16    rk     Abstracting OSAL layer
09/01/14    rp     replaced the qmi debug buffer with uniform csd debug buffer
30/06/14    rp     Added support for Calibration Feature ID 
07/03/13    vk     Changes to remove compiler warnings
05/24/13    rp     Added session name logging 
                   Added support for asynchronized qmi csd clients apis
06/26/12    jk     Added support for not calling callback when client is disconnected 
02/22/12    jp/rp  Corrected the typo for disbale command
02/10/12    jp     Corrected message printf syntx in volume
12/06/11    jp     Added Support for Smooth volume and mute features
09/12/11    akj    Created file (initial version)

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "csd_qmi_services_vc.h"
#include "csd_vc_ioctl.h"
#include "csd_vc.h"
#include "apr_errcodes.h"
#include "csd_debug_info.h"
#include "csd_os_dependencies.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/* This varaible is used to store the QMI client info. When multiple sessions
    of VC is called, the variable will be overwritten erasing the other session
    client info. we will have to handle this senario properly*/
static csd_cb_data_type csd_vc_cb_data;
extern csd_event_t qmi_csd_sync_event;
/*=============================================================================
                    QMI CSD Voice Context Callback Functions
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_vc_callback
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

uint32_t qmi_csd_vc_callback(uint32_t cmd_token, uint32_t event_id,
                         void* param, uint32_t param_size, void* cb_data_info)
{
  uint32_t cmd_param_cmd_token, cmd_param_cmd_type, hdl, size;
  csd_cmd_token_param_t cmd_param_info;
  qmi_client_handle clnt_hdl;

  qmi_req_handle req_handle;
  qmi_csd_vc_cmd_resp_msg *resp_msg;
  csd_vc_event_command_rsp_t *rc_status;

  csd_vc_event_tx_dtmf_detected_t* rsp;

  qmi_csd_ioctl_vc_tx_dtmf_detected_ind_msg_v01 ind_info;

  int32_t msg_id, resp_len=0;
  uint32_t rc = CSD_EFAILED;
  qmi_csi_error resp_err;

  csd_cb_data_type *cb_data_ptr = (csd_cb_data_type*)cb_data_info;
  hdl = cb_data_ptr->handle;
  clnt_hdl = (cb_data_ptr->clnt_info_data)->clnt;

  switch( event_id )
  {
    case CSD_VC_EVENTID_COMMAND_RSP:
    {
      rc_status = (csd_vc_event_command_rsp_t*)param;

      cmd_param_info.cmd_token = cmd_token;
      rc = qmi_csd_client_info_get_list ( &cmd_param_info);
      if (CSD_EOK == rc)
      {
        cmd_param_cmd_type = cmd_param_info.cmd_type;
        cmd_param_cmd_token = cmd_param_info.cmd_token;
        req_handle = cmd_param_info.req_handle;
        msg_id = cmd_param_info.msg_id;

        resp_msg = (qmi_csd_vc_cmd_resp_msg*)malloc( sizeof(qmi_csd_vc_cmd_resp_msg) );
        if( resp_msg != NULL )
        {
          memset( resp_msg, 0, sizeof(*resp_msg) );

          if( rc_status != NULL )
          {
            rc = rc_status->status_id;
          }

          switch( cmd_param_cmd_type )
          {
            case CSD_VC_IOCTL_SET_DEVICE_CONFIG:
              resp_len = sizeof(resp_msg->u.qmi_csd_set_device_config_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_device_config_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_device_config_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_device_config_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_device_config_resp.cmd_token_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_DEVICE_CONFIG failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_device_config_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_device_config_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_device_config_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_device_config_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_DEVICE_CONFIG success" );
              }        
            break;

            case CSD_VC_IOCTL_ENABLE:
              resp_len = sizeof (resp_msg->u.qmi_csd_enable_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_enable_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_enable_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_enable_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_enable_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_enable_resp.handle_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_ENABLE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_enable_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_enable_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_enable_resp.handle = hdl;
                resp_msg->u.qmi_csd_enable_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_enable_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_ENABLE success" );
              }
            break;
            

            case CSD_VC_IOCTL_DISABLE:
              resp_len = sizeof(resp_msg->u.qmi_csd_disable_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_disable_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_disable_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_disable_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_disable_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_disable_resp.handle_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_DISABLE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_disable_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_disable_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_disable_resp.handle = hdl;
                resp_msg->u.qmi_csd_disable_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_disable_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_DISABLE success" );
              }            
            break;

            case CSD_VC_IOCTL_SET_RX_VOLUME_INDEX:
              resp_len = sizeof(resp_msg->u.qmi_csd_set_rx_volume_resp);

              /* TODO: This is hardcoded for test tool to carry on with voice call when no volume 
                 cal data is there in ACDB, it should be remooved once tool is able to allow this behavior. */
              rc = CSD_EOK;
              
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_rx_volume_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_rx_volume_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_rx_volume_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_rx_volume_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_rx_volume_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_rx_volume_resp.cmd_token_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_RX_VOLUME_INDEX failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_rx_volume_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_rx_volume_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_rx_volume_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_rx_volume_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_rx_volume_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_RX_VOLUME_INDEX success" );
              }
            break;

            case CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_volume_steps_resp);

              if( rc != CSD_EOK )
              {
                resp_msg->u.qmi_csd_set_volume_steps_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_volume_steps_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_volume_steps_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_volume_steps_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_volume_steps_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_volume_steps_resp.cmd_token_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_volume_steps_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_volume_steps_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_volume_steps_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_volume_steps_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_volume_steps_resp.cmd_token = cmd_param_cmd_token;
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS success" );
              }
            break;

            case CSD_VC_IOCTL_SET_RX_VOLUME_STEP:
              resp_len = sizeof(resp_msg->u.qmi_csd_set_rx_volume_step_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.cmd_token_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_RX_VOLUME_STEP failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_rx_volume_step_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_RX_VOLUME_STEP success" );
              }    
            break;

            case CSD_VC_IOCTL_SET_MUTE:
              resp_len = sizeof (resp_msg->u.qmi_csd_set_mute_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_mute_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_mute_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_mute_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_mute_resp.cmd_token_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_MUTE failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_mute_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_mute_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_mute_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_mute_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_MUTE success" );
              }
            break;

            case CSD_VC_IOCTL_SET_TX_DTMF_DETECTION:
              resp_len = sizeof (resp_msg->u.qmi_csd_tx_dtmf_detection_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_TX_DTMF_DETECTION failed, rc[0x%x]", rc );
              }
              else 
              {
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.handle = hdl;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_tx_dtmf_detection_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_TX_DTMF_DETECTION success" );
              } 
            break;

            case CSD_VC_IOCTL_SET_UI_PROPERTY:
              resp_len = sizeof(resp_msg->u.qmi_csd_set_ui_property_resp);

              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_set_ui_property_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_set_ui_property_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_set_ui_property_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_set_ui_property_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_set_ui_property_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_set_ui_property_resp.cmd_token_valid = FALSE;
                
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_UI_PROPERTY failed, rc[0x%x]", rc );
              }
              else 
              { 
                resp_msg->u.qmi_csd_set_ui_property_resp.qmi_csd_status_code_valid = FALSE;
                resp_msg->u.qmi_csd_set_ui_property_resp.handle_valid = TRUE;
                resp_msg->u.qmi_csd_set_ui_property_resp.handle = hdl;
                resp_msg->u.qmi_csd_set_ui_property_resp.cmd_token_valid = TRUE;
                resp_msg->u.qmi_csd_set_ui_property_resp.cmd_token = cmd_param_cmd_token;
                
                CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_UI_PROPERTY success" );
              }
            break;

            case CSD_VC_IOCTL_GET_UI_PROPERTY:
            {
              csd_vc_ioctl_get_ui_property_t *svc_csd_vc_param = NULL;
              qmi_csd_vc_ioctl_get_ui_property_resp_v01 *payload_ptr;

              svc_csd_vc_param = (csd_vc_ioctl_get_ui_property_t*) param;
              if (svc_csd_vc_param != NULL)
              {
                resp_len = sizeof(qmi_csd_ioctl_vc_cmd_get_ui_property_resp_msg_v01);
                if( rc != CSD_EOK ) 
                {
                  resp_msg->u.qmi_csd_get_ui_property_resp.resp.result = QMI_RESULT_FAILURE_V01;
                  resp_msg->u.qmi_csd_get_ui_property_resp.resp.error = QMI_ERR_GENERAL_V01;
                  resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_status_code_valid = TRUE;
                  resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                  resp_msg->u.qmi_csd_get_ui_property_resp.handle_valid = FALSE;
                  resp_msg->u.qmi_csd_get_ui_property_resp.cmd_token_valid = FALSE;
                  resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_vc_ioctl_get_ui_property_resp_payload_valid = FALSE;
                  
                  CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_GET_UI_PROPERTY failed, rc[0x%x]", rc );
                }
                else 
                { 
                  resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_status_code_valid = FALSE;
                  resp_msg->u.qmi_csd_get_ui_property_resp.handle_valid = TRUE;
                  resp_msg->u.qmi_csd_get_ui_property_resp.handle = hdl;
                  resp_msg->u.qmi_csd_get_ui_property_resp.cmd_token_valid = TRUE;
                  resp_msg->u.qmi_csd_get_ui_property_resp.cmd_token = cmd_param_cmd_token;
                  resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_vc_ioctl_get_ui_property_resp_payload_valid = TRUE;
                  
                  CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_GET_UI_PROPERTY success" );

                  payload_ptr = (qmi_csd_vc_ioctl_get_ui_property_resp_v01*)&(resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_vc_ioctl_get_ui_property_resp_payload);

                  // svc_csd_vs_param now point to memory with UI property structure filled in.
                  size = svc_csd_vc_param->param_size;
                  payload_ptr->cmd_token = cmd_param_cmd_token;
                  payload_ptr->module_id = svc_csd_vc_param->module_id;
                  payload_ptr->param_id = svc_csd_vc_param->param_id ;
                  payload_ptr->param_data_len = size ;
                  csd_memscpy( payload_ptr->param_data, sizeof(payload_ptr->param_data),
                               svc_csd_vc_param->param_data, size );
                }
                free(svc_csd_vc_param->param_data);
              }
              else
              {
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_GET_UI_PROPERTY param data allocation failed" );
              }
              break;
            }
            
            case CSD_VC_IOCTL_SET_CAL_FID:
              resp_len = sizeof(resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp);
              if( rc != CSD_EOK ) 
              {
                resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.resp.result = QMI_RESULT_FAILURE_V01;
                resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.resp.error = QMI_ERR_GENERAL_V01;
                resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.qmi_csd_status_code_valid = TRUE;
                resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.qmi_csd_status_code = (qmi_csd_status_v01) rc;
                resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.handle_valid = FALSE;
                resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.cmd_token_valid = FALSE;
                CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_CAL_FID failed, rc[0x%x]", rc );
              }
              else 
              { 
                 resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.qmi_csd_status_code_valid = FALSE;
                 resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.handle_valid = TRUE;
                 resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.handle = hdl;
                 resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.cmd_token_valid = TRUE;
                 resp_msg->u.qmi_csd_vc_set_cal_feature_id_resp.cmd_token = cmd_param_cmd_token;
                 CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: CSD_VC_IOCTL_SET_CAL_FID success" );
               }
             break;
            default:
              rc = CSD_ENOTFOUND;
              CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: Unsupported cmd_param_cmd_type[0x%x]", cmd_param_cmd_type );
            break;
          } // switch cmd_type

          if( rc != CSD_ENOTFOUND )
          {       
            resp_err = qmi_csi_send_resp( req_handle, msg_id, resp_msg, resp_len );
            if( resp_err != QMI_CSI_NO_ERR )
            {
              CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: qmi_csi_send_resp failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
              rc = QMI_CSI_CB_INTERNAL_ERR;
            }
            else
            {
              CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
              QMI_CSD_RESP_SET_BUFFER(msg_id, rc );
            }

            csd_event_signal(qmi_csd_sync_event);              
          }
          free(resp_msg);
        } //if end resp_msg
        else
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: resp memory allocation failed" );
        }
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR,"qmi_csd_vc_callback: could not get valid list item for given token \n");
      }
      break;
    } // end case CSD_VC_EVENTID_COMMAND_RSP
    
    case CSD_VC_EVENTID_TX_DTMF_DETECTED:
    {
      rsp = (csd_vc_event_tx_dtmf_detected_t*)param;
      rc = (uint32_t) rsp->low_freq;

      ind_info.handle = hdl;
      ind_info.dtmf_low_freq = rsp->low_freq;
      ind_info.dtmf_high_freq = rsp->high_freq;
      msg_id = QMI_CSD_IOCTL_VC_TX_DTMF_DETECTED_IND_V01;

      CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_vc_callback: DTMF tone Detected, high_freq[%d], low_freq[%d]", rsp->high_freq, rsp->low_freq );

      resp_err = qmi_csi_send_ind( clnt_hdl, msg_id, &ind_info, sizeof(ind_info) );

      if( resp_err != QMI_CSI_NO_ERR )
      {
        rc = QMI_CSI_CB_INTERNAL_ERR;
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: qmi_csi_send_ind failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
      }
      break;
    }

    case CSD_VC_EVENTID_STATE_NOTIFY:
      rc = CSD_EOK;
    break;

    default:
      rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_vc_callback: Unknown event_id[0x%x]", event_id );
    break;
  } // switch for event_id

   return rc;
}

/*=============================================================================
  FUNCTION csd_open_voice_context_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_OPEN_VOICE_CONTEXT_REQ
  message is received by the service, sending a response message with the voice
  context handler.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error csd_open_voice_context_req_handler
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
  qmi_csd_vc_open_full_control_type_v01 *req_ptr;
  qmi_csd_open_voice_context_resp_msg_v01 resp;
  char_t voice_session_name[SESSION_NAME_MAX_LEN_V01 + 1];
  uint32_t size, vc_handle;
  uint32_t csd_rc = CSD_EOK;

  csd_vc_open_full_control_t qmi_csd_svc_open_vc_info;

  req_ptr = &(((qmi_csd_open_voice_context_req_msg_v01*)(req_c_struct))->qmi_csd_vc_open_payload);

  csd_vc_cb_data.clnt_info_data = clnt_info;
  qmi_csd_svc_open_vc_info.callback_fn = qmi_csd_vc_callback;
  qmi_csd_svc_open_vc_info.cb_data = (void* )&csd_vc_cb_data;

  size = strlcpy(voice_session_name, req_ptr->session_name, sizeof(voice_session_name));
  qmi_csd_svc_open_vc_info.name = voice_session_name;
  qmi_csd_svc_open_vc_info.name_size = (size+1);
  qmi_csd_svc_open_vc_info.ret_status = &csd_rc;
  qmi_csd_svc_open_vc_info.direction = (uint16_t)(req_ptr->direction);
  qmi_csd_svc_open_vc_info.network_id = (uint32_t)(req_ptr->network_id);

  CSD_MSG( CSD_PRIO_HIGH, "csd_open_voice_context_req_handler: QMI_CSD_OPEN_VOICE_CONTEXT_REQ_V01 called" );   
  
  vc_handle = csd_open( CSD_OPEN_VOICE_CONTEXT, &qmi_csd_svc_open_vc_info, 
                        sizeof(qmi_csd_svc_open_vc_info) );

  csd_vc_cb_data.handle = vc_handle;

  memset( &resp, 0, sizeof(qmi_csd_open_voice_context_resp_msg_v01) );

  if( vc_handle == 0 )
  {
    csd_rc = CSD_EFAILED;
    CSD_MSG( CSD_PRIO_ERROR, "csd_open_voice_context_req_handler: CSD_OPEN_VOICE_CONTEXT failed" );
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    resp.open_status_valid = TRUE;
    resp.open_status = (qmi_csd_status_v01)csd_rc;
    resp.qmi_csd_vc_handle_valid = FALSE;
  }
  else 
  {     
    CSD_MSG( CSD_PRIO_HIGH, "csd_open_voice_context_req_handler: CSD_OPEN_VOICE_CONTEXT success, vc_handle[0x%x]", vc_handle );
    resp.open_status_valid = FALSE;
    resp.qmi_csd_vc_handle_valid = TRUE;
    resp.qmi_csd_vc_handle = vc_handle;
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_open_voice_context_req_handler: qmi_csi_send_resp failed, rc[0x%x]", resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_open_voice_context_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
    QMI_CSD_RESP_SET_BUFFER(msg_id, vc_handle );
  }
  return qmi_rc;
}


/*=============================================================================
  FUNCTION csd_ioctl_vc_cmd_req_handler
=============================================================================*/
/*!
@brief
  This function calls the CSD VC IOCTL for the QMI message recieved.

@param[in]  handle               CSD VC handle pertaining to a particular
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
qmi_csi_cb_error qmi_csd_ioctl_vc_cmd_req_handler
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
  qmi_csi_error resp_err;
  uint32_t resp_msg_size = 0;
  csd_cmd_token_param_t cmd_info;
  qmi_csd_vc_cmd_req_msg *qmi_csd_vc_req = (qmi_csd_vc_cmd_req_msg*)req_c_struct;
  service_context_type *context = (service_context_type*)service_cookie;
  qmi_csd_vc_cmd_resp_msg *resp_msg;
  uint32_t vc_hdl= handle;
  uint32_t client_cmd_token;
  int32_t apr_rc = APR_EOK;
  
  memset ( &cmd_info, 0, sizeof ( csd_cmd_token_param_t ) );
  cmd_info.cmd_type = cmd_msg_id;
  cmd_info.clnt_info = clnt_info;
  cmd_info.req_handle = req_handle;
  cmd_info.msg_id = msg_id;
  cmd_info.service_cookie = context;
  cmd_info.cmd_resp_ptr = NULL;
  

  resp_msg = (qmi_csd_vc_cmd_resp_msg*)malloc( sizeof(qmi_csd_vc_cmd_resp_msg) );
  if( resp_msg !=NULL )
  {
    switch( cmd_msg_id )
    {
      case CSD_VC_IOCTL_SET_UI_PROPERTY:
      {
        csd_vc_ioctl_set_ui_property_t qmi_csd_req_info;
        csd_cmd_param_UI_property_data_t param_payload;
        qmi_csd_vc_ioctl_set_ui_property_v01 *req_info = (qmi_csd_vc_ioctl_set_ui_property_v01 *)(&(qmi_csd_vc_req->u.qmi_csd_set_ui_property_req.qmi_csd_vc_ioctl_set_ui_property_payload));

        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_set_ui_property_req.qmi_csd_vc_ioctl_set_ui_property_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info );

        qmi_csd_req_info.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info.module_id = req_info->module_id;
        qmi_csd_req_info.param_id  = req_info->param_id;
        qmi_csd_req_info.param_size = req_info->param_data_len + ( ( 4 - ( req_info->param_data_len & 0x00000003)) & 0x00000003);
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_UI_PROPERTY called, module[0x%x] param[0x%x] data_len[0x%x] param_size[0x%x]", 
                 req_info->module_id, req_info->param_id, req_info->param_data_len, qmi_csd_req_info.param_size );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_memscpy( param_payload.param_data, sizeof(param_payload.param_data),
                     req_info->param_data, qmi_csd_req_info.param_size );
        qmi_csd_req_info.param_data = &param_payload.param_data;

        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_UI_PROPERTY, &qmi_csd_req_info, sizeof(csd_vc_ioctl_set_ui_property_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_UI_PROPERTY failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_ui_property_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_ui_property_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_ui_property_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_ui_property_resp.qmi_csd_status_code = (qmi_csd_status_v01)qmi_rc;
          resp_msg->u.qmi_csd_set_ui_property_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_ui_property_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_ui_property_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_GET_UI_PROPERTY:
      {
        csd_vc_ioctl_get_ui_property_t qmi_csd_req_info;
        qmi_csd_vc_ioctl_get_ui_property_req_v01 *req_info = (qmi_csd_vc_ioctl_get_ui_property_req_v01 *)(&(qmi_csd_vc_req->u.qmi_csd_get_ui_property_req.qmi_csd_vc_ioctl_get_ui_property_req_payload));

        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_get_ui_property_req.qmi_csd_vc_ioctl_get_ui_property_req_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info);

        qmi_csd_req_info.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info.module_id = req_info->module_id;
        qmi_csd_req_info.param_id  = req_info->param_id;

        qmi_csd_req_info.param_data = (csd_cmd_param_UI_property_data_t*)malloc(sizeof(csd_cmd_param_UI_property_data_t));
        qmi_csd_req_info.param_size = sizeof( csd_cmd_param_UI_property_data_t );

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_GET_UI_PROPERTY called, module[0x%x] param[0x%x] param_size[0x%x]",
                 req_info->module_id, req_info->param_id, qmi_csd_req_info.param_size );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_GET_UI_PROPERTY, &qmi_csd_req_info, sizeof(csd_vc_ioctl_get_ui_property_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_GET_UI_PROPERTY failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_get_ui_property_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_get_ui_property_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_get_ui_property_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_get_ui_property_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_get_ui_property_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_get_ui_property_resp);
          free( qmi_csd_req_info.param_data );
        }
      }
      break;

      case CSD_VC_IOCTL_SET_RX_VOLUME_INDEX:
      {
        csd_vc_ioctl_set_rx_volume_index_t qmi_csd_req_info_volume;
        qmi_csd_vc_ioctl_set_rx_volume_index_v01 *req_info_volume = (qmi_csd_vc_ioctl_set_rx_volume_index_v01 *)(&(qmi_csd_vc_req->u.qmi_csd_set_rx_volume_req.qmi_csd_vc_ioctl_set_rx_volume_index_payload));

        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_set_rx_volume_req.qmi_csd_vc_ioctl_set_rx_volume_index_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_req_info_volume.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_volume.vol_index = req_info_volume->vol_index;

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_RX_VOLUME_INDEX called, vol_index[%d]", req_info_volume->vol_index );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_RX_VOLUME_INDEX, &qmi_csd_req_info_volume, sizeof(csd_vc_ioctl_set_rx_volume_index_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "CSD_VC_IOCTL_SET_RX_VOLUME_INDEX failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_rx_volume_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_rx_volume_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_rx_volume_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_rx_volume_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_set_rx_volume_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_rx_volume_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_rx_volume_resp);
        }
      }
      break;
    
      case CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS:
      {
        csd_vc_ioctl_set_number_of_steps_t qmi_csd_req_info_num_of_volume_steps;
        qmi_csd_vc_ioctl_set_number_of_volume_steps_v01 *req_info_volume_steps = (qmi_csd_vc_ioctl_set_number_of_volume_steps_v01 *)(&(qmi_csd_vc_req->u.qmi_csd_set_volume_steps_req.qmi_csd_vc_ioctl_set_number_of_volume_steps_payload));
        
        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_set_volume_steps_req.qmi_csd_vc_ioctl_set_number_of_volume_steps_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_req_info_num_of_volume_steps.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_num_of_volume_steps.value = req_info_volume_steps->value;

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS called, num_of_volume_steps[%d]", req_info_volume_steps->value );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS, &qmi_csd_req_info_num_of_volume_steps, sizeof(csd_vc_ioctl_set_number_of_steps_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_volume_steps_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_volume_steps_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_volume_steps_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_volume_steps_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_set_volume_steps_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_volume_steps_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_volume_steps_resp);
        }
      }
      break;
    
      case CSD_VC_IOCTL_SET_RX_VOLUME_STEP:
      {
        csd_vc_ioctl_set_rx_volume_step_t qmi_csd_rx_volume_step;
        qmi_csd_vc_ioctl_set_rx_volume_step_v01 *req_info_rx_volume_step = (qmi_csd_vc_ioctl_set_rx_volume_step_v01 *)(&(qmi_csd_vc_req->u.qmi_csd_set_rx_volume_step_req.qmi_csd_vc_ioctl_set_rx_volume_step_payload));

        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_set_rx_volume_step_req.qmi_csd_vc_ioctl_set_rx_volume_step_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( &cmd_info );

        qmi_csd_rx_volume_step.cmd_token = cmd_info.cmd_token;
        qmi_csd_rx_volume_step.vol_step = req_info_rx_volume_step->vol_step;
        qmi_csd_rx_volume_step.ramp_duration = req_info_rx_volume_step->ramp_duration;

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_RX_VOLUME_STEP called, volume_step[%d] ramp_duration[%d]", req_info_rx_volume_step->vol_step, req_info_rx_volume_step->ramp_duration );
        CSD_MSG( CSD_PRIO_LOW, "client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_RX_VOLUME_STEP, &qmi_csd_rx_volume_step, sizeof(csd_vc_ioctl_set_rx_volume_step_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_RX_VOLUME_STEP failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_rx_volume_step_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_rx_volume_step_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_rx_volume_step_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_rx_volume_step_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_set_rx_volume_step_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_rx_volume_step_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_rx_volume_step_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_SET_MUTE:
      {
        csd_vc_ioctl_set_mute_v2_t qmi_csd_req_info_mute;
        qmi_csd_vc_ioctl_set_mute_v01 *req_info_mute = (qmi_csd_vc_ioctl_set_mute_v01 *)(&(qmi_csd_vc_req->u.qmi_csd_set_mute_req.qmi_csd_vc_ioctl_set_mute_payload));
        
        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_set_mute_req.qmi_csd_vc_ioctl_set_mute_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_req_info_mute.cmd_token = cmd_info.cmd_token;
        qmi_csd_req_info_mute.direction = (uint16_t)req_info_mute->direction;
        qmi_csd_req_info_mute.mute_flag = (uint16_t)req_info_mute->mute_flag;
        qmi_csd_req_info_mute.ramp_duration = 0;

        if ( qmi_csd_vc_req->u.qmi_csd_set_mute_req.ramp_duration_valid == TRUE)
        {
          qmi_csd_req_info_mute.ramp_duration = qmi_csd_vc_req->u.qmi_csd_set_mute_req.ramp_duration;  
        }
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_MUTE called, direction[%d] mute_flag[%d] ramp_duration[%d]", req_info_mute->direction, req_info_mute->mute_flag, qmi_csd_req_info_mute.ramp_duration );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_MUTE_V2, &qmi_csd_req_info_mute, sizeof(csd_vc_ioctl_set_mute_v2_t) );
        
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_MUTE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_mute_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_mute_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_mute_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_set_mute_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_mute_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_mute_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_ENABLE:
      {
        uint32_t qmi_csd_svc_vc_enable_arg;
        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_enable_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_svc_vc_enable_arg = cmd_info.cmd_token;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_ENABLE called" );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_ENABLE, &qmi_csd_svc_vc_enable_arg, sizeof(uint32_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_ENABLE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_enable_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_enable_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_enable_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_enable_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_enable_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_enable_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_enable_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_DISABLE:
      {
        uint32_t qmi_csd_svc_vc_disable_arg;
        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_disable_req.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_svc_vc_disable_arg = cmd_info.cmd_token;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_DISABLE called" );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_DISABLE, &qmi_csd_svc_vc_disable_arg, sizeof(uint32_t) );
        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_DISABLE failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_disable_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_disable_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_disable_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_disable_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_disable_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_disable_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_disable_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_SET_DEVICE_CONFIG:
      {
        csd_vc_ioctl_set_device_config_v2_t qmi_csd_dev_config ;
        qmi_csd_vc_ioctl_set_device_config_v01 *req_info_set_dev_config = (qmi_csd_vc_ioctl_set_device_config_v01*)(&(qmi_csd_vc_req->u.qmi_csd_set_device_config_req.qmi_csd_vc_ioctl_set_device_config_payload));

        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_set_device_config_req.qmi_csd_vc_ioctl_set_device_config_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_dev_config.cmd_token = cmd_info.cmd_token;
        qmi_csd_dev_config.rx_dev_id = req_info_set_dev_config->rx_dev_num;
        qmi_csd_dev_config.tx_dev_id = req_info_set_dev_config->tx_dev_num;
        qmi_csd_dev_config.rx_dev_sr  = req_info_set_dev_config->rx_dev_sr;
        qmi_csd_dev_config.tx_dev_sr  = req_info_set_dev_config->tx_dev_sr;
        qmi_csd_dev_config.ec_ref_dev_id = 0xFFFFFFFF;

        if( qmi_csd_vc_req->u.qmi_csd_set_device_config_req.ec_ref_dev_num_valid == TRUE )
        {
          qmi_csd_dev_config.ec_ref_dev_id = qmi_csd_vc_req->u.qmi_csd_set_device_config_req.ec_ref_dev_num;          
        }

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_DEVICE_CONFIG called, rx_dev_id[0x%x] tx_dev_id[0x%x] rx_sr[%d] tx_sr[%d] rc_ref_id[0x%x]",
                 req_info_set_dev_config->rx_dev_num, req_info_set_dev_config->tx_dev_num, req_info_set_dev_config->rx_dev_sr, 
                 req_info_set_dev_config->tx_dev_sr, qmi_csd_dev_config.ec_ref_dev_id );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        
        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2, &qmi_csd_dev_config, sizeof(csd_vc_ioctl_set_device_config_v2_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_DEVICE_CONFIG failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_device_config_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_device_config_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_set_device_config_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_device_config_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_device_config_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_SET_TX_DTMF_DETECTION:
      {
        csd_vc_ioctl_set_tx_dtmf_detection_t qmi_csd_dtmf_detect;
        qmi_csd_vc_ioctl_tx_dtmf_detect_v01 *req_info_dtmf_detect = (qmi_csd_vc_ioctl_tx_dtmf_detect_v01*)(&(qmi_csd_vc_req->u.qmi_csd_tx_dtmf_detection_req.qmi_csd_vc_ioctl_tx_dtmf_detect_payload));

        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_tx_dtmf_detection_req.qmi_csd_vc_ioctl_tx_dtmf_detect_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;

        csd_rc = qmi_csd_client_info_set_list( & cmd_info );

        qmi_csd_dtmf_detect.cmd_token = cmd_info.cmd_token;
        qmi_csd_dtmf_detect.enable    = req_info_dtmf_detect->enable;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_TX_DTMF_DETECTION called, enable[%d]", req_info_dtmf_detect->enable );
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );

        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_TX_DTMF_DETECTION, &qmi_csd_dtmf_detect, sizeof(csd_vc_ioctl_set_tx_dtmf_detection_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_TX_DTMF_DETECTION failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_tx_dtmf_detection_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_tx_dtmf_detection_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_tx_dtmf_detection_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_tx_dtmf_detection_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_tx_dtmf_detection_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_tx_dtmf_detection_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_tx_dtmf_detection_resp);
        }
      }
      break;

      case CSD_VC_IOCTL_SET_CAL_FID:
      {
        csd_vc_ioctl_set_cal_fid_t qmi_csd_vc_set_cal_fid ;
        qmi_csd_vc_set_cal_feature_id_v01 *req_info_set_cal_fid = (qmi_csd_vc_set_cal_feature_id_v01*)
              (&(qmi_csd_vc_req->u.qmi_csd_vc_set_cal_feature_id_req.qmi_csd_vc_set_cal_feature_id_payload));
        cmd_info.cmd_token = qmi_csd_vc_req->u.qmi_csd_vc_set_cal_feature_id_req.qmi_csd_vc_set_cal_feature_id_payload.cmd_token;
        client_cmd_token = cmd_info.cmd_token;
        csd_rc = qmi_csd_client_info_set_list( &cmd_info );
        qmi_csd_vc_set_cal_fid.cmd_token = cmd_info.cmd_token;
        qmi_csd_vc_set_cal_fid.num_fid_in_table = 1 ; /*support only one feature ID at a time*/
        qmi_csd_vc_set_cal_fid.fid_table[0].feature_id = (uint32_t)req_info_set_cal_fid->cal_feature_id.feature_id;
        qmi_csd_vc_set_cal_fid.fid_table[0].cal_table  = (csd_vc_ioctl_calibration_table_enum_t)
                                                req_info_set_cal_fid->cal_feature_id.feature_type;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_CAL_FID called, feature_id[0x%x] feature_type[0x%x] ",
                   req_info_set_cal_fid->cal_feature_id.feature_id, req_info_set_cal_fid->cal_feature_id.feature_type);
        CSD_MSG( CSD_PRIO_LOW, "qmi_csd_ioctl_vc_cmd_req_handler: client_cmd_token[0x%x] csd_cmd_token[0x%x]", client_cmd_token, cmd_info.cmd_token );
        csd_rc = csd_ioctl( vc_hdl, CSD_VC_IOCTL_SET_CAL_FID, &qmi_csd_vc_set_cal_fid, sizeof(csd_vc_ioctl_set_cal_fid_t) );
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: CSD_VC_IOCTL_SET_CAL_FID failed, csd_rc[0x%x]", csd_rc );
          resp_msg->u.qmi_csd_set_device_config_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_set_device_config_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_set_device_config_resp.qmi_csd_status_code = (qmi_csd_status_v01) qmi_rc;
          resp_msg->u.qmi_csd_set_device_config_resp.handle_valid = FALSE;
          resp_msg->u.qmi_csd_set_device_config_resp.cmd_token_valid = FALSE;
          resp_msg_size = sizeof(resp_msg->u.qmi_csd_set_device_config_resp);
        }
       }
       break;
      default:
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: Unknown cmd_msg_id[0x%x]", cmd_msg_id );
    }

    /* only when error happens, send response message here.
       For success case response will be sent in the event callbacks. */
    if( csd_rc != CSD_EOK )
    {
      resp_err = qmi_csi_send_resp( req_handle, msg_id, resp_msg, resp_msg_size );
      if( resp_err != QMI_CSI_NO_ERR )
      {
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: qmi_csi_send_resp failed, resp_err[0x%x]", resp_err );
      }
      else
      {
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_vc_cmd_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
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
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: event wait failed, rc[%d]", apr_rc );
      }
    }
    free( resp_msg );
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_vc_cmd_req_handler: resp memory allocation falied" );
  }
  return qmi_rc;
}
