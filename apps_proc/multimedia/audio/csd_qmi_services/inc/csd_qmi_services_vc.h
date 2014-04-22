#ifndef __AUDIO_CSD_QMI_SERVICES_VC_H__
#define __AUDIO_CSD_QMI_SERVICES_VC_H__
/*===========================================================================

 A U D I O _ C S D _ Q M I _ S E R V I C E _ V C   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_vc.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
30/06/14    rp      Added support for Calibration Feature ID 
12/06/11    jp      Added new messages for smooth volume
09/06/11    akj     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/
#include "csd_qmi_services_common.h"
#include "core_sound_driver_v01.h"
#include "csd.h"
#include "csd_vcommon.h"
#include "csd_debug_msg.h"

/****************************************************************************
   DEFINES
****************************************************************************/


/*===========================================================================
 Function Prototype

===========================================================================*/

typedef struct _qmi_csd_vc_cmd_resp_msg_v01
{
	union
	{
		qmi_csd_ioctl_vc_cmd_get_ui_property_resp_msg_v01 qmi_csd_get_ui_property_resp;
		qmi_csd_ioctl_vc_cmd_set_ui_property_resp_msg_v01 qmi_csd_set_ui_property_resp;
		qmi_csd_ioctl_vc_cmd_set_tx_dtmf_detection_resp_msg_v01 qmi_csd_tx_dtmf_detection_resp;
		qmi_csd_ioctl_vc_cmd_set_mute_resp_msg_v01 qmi_csd_set_mute_resp;
		qmi_csd_ioctl_vc_cmd_set_rx_volume_index_resp_msg_v01 qmi_csd_set_rx_volume_resp;
		qmi_csd_ioctl_vc_cmd_disable_resp_msg_v01 qmi_csd_disable_resp;
		qmi_csd_ioctl_vc_cmd_enable_resp_msg_v01 qmi_csd_enable_resp;
		qmi_csd_ioctl_vc_cmd_set_device_config_resp_msg_v01 qmi_csd_set_device_config_resp;
		qmi_csd_open_voice_context_resp_msg_v01 qmi_csd_open_context_resp;
		qmi_csd_ioctl_vc_cmd_set_number_of_volume_steps_resp_msg_v01 qmi_csd_set_volume_steps_resp;
		qmi_csd_ioctl_vc_cmd_set_rx_volume_step_resp_msg_v01 qmi_csd_set_rx_volume_step_resp;
        qmi_csd_vc_set_cal_feature_id_resp_msg_v01  qmi_csd_vc_set_cal_feature_id_resp;
	}u;
}qmi_csd_vc_cmd_resp_msg;

typedef struct _qmi_csd_vc_cmd_req_msg_v01
{
	union
	{
		qmi_csd_ioctl_vc_cmd_get_ui_property_req_msg_v01 qmi_csd_get_ui_property_req;
		qmi_csd_ioctl_vc_cmd_set_ui_property_req_msg_v01 qmi_csd_set_ui_property_req;
		qmi_csd_ioctl_vc_cmd_set_tx_dtmf_detection_req_msg_v01 qmi_csd_tx_dtmf_detection_req;
		qmi_csd_ioctl_vc_cmd_set_mute_req_msg_v01 qmi_csd_set_mute_req;
		qmi_csd_ioctl_vc_cmd_set_rx_volume_index_req_msg_v01 qmi_csd_set_rx_volume_req;
		qmi_csd_ioctl_vc_cmd_disable_req_msg_v01 qmi_csd_disable_req;
		qmi_csd_ioctl_vc_cmd_enable_req_msg_v01 qmi_csd_enable_req;
		qmi_csd_ioctl_vc_cmd_set_device_config_req_msg_v01 qmi_csd_set_device_config_req;
		qmi_csd_open_voice_context_req_msg_v01 qmi_csd_open_context_req;
		qmi_csd_ioctl_vc_cmd_set_number_of_volume_steps_req_msg_v01 qmi_csd_set_volume_steps_req;
		qmi_csd_ioctl_vc_cmd_set_rx_volume_step_req_msg_v01 qmi_csd_set_rx_volume_step_req;
        qmi_csd_vc_set_cal_feature_id_req_msg_v01     qmi_csd_vc_set_cal_feature_id_req;
	}u;
}qmi_csd_vc_cmd_req_msg;

uint32_t qmi_csd_vc_callback 
( 
  uint32_t cmd_token, uint32_t event_id,
  void* param, uint32_t param_size, void* cb_data_info
);
							   
qmi_csi_cb_error csd_open_voice_context_req_handler 
( client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
);							   

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
);
#endif /* __AUDIO_CSD_QMI_SERVICES_VC_H__*/

