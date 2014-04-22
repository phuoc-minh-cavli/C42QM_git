#ifndef __AUDIO_CSD_QMI_SERVICES_VS_H__
#define __AUDIO_CSD_QMI_SERVICES_VS_H__
/*===========================================================================

  A U D I O _ C S D _ Q M I _ S E R V I C E _ V S   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_vs.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/11    jp      Added Music delivery messages
09/06/11    akj     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/


#include "csd_qmi_services_common.h"
#include "csd.h"
#include "csd_vs_ioctl.h"
#include "csd_status.h"
#include "csd_debug_msg.h"



/****************************************************************************
   DEFINES
****************************************************************************/


/*===========================================================================
 Function Prototype

===========================================================================*/

typedef struct _qmi_csd_vs_cmd_resp_msg_v01
{
	union
	{
		qmi_csd_ioctl_vs_cmd_set_media_type_resp_msg_v01 qmi_csd_set_media_type_resp;
		qmi_csd_ioctl_vs_cmd_set_mute_resp_msg_v01 qmi_csd_set_mute_resp;
		qmi_csd_ioctl_vs_cmd_set_encoder_dtx_mode_resp_msg_v01 qmi_csd_enc_dtx_mode_resp;
		qmi_csd_ioctl_vs_cmd_set_dec_timewarp_resp_msg_v01 qmi_csd_dec_timewarp_resp;
		qmi_csd_ioctl_vs_cmd_set_enc_minmax_rate_resp_msg_v01 qmi_csd_enc_minmax_rate_resp;
		qmi_csd_ioctl_vs_cmd_set_enc_rate_modulation_resp_msg_v01 qmi_csd_enc_rate_modulation_resp;
		qmi_csd_ioctl_vs_cmd_voc_qcelp13k_set_rate_resp_msg_v01 qmi_csd_qcelp_set_rate_resp;
		qmi_csd_ioctl_vs_cmd_voc_4gvnb_set_rate_resp_msg_v01 qmi_csd_4gvnb_set_rate_resp;
		qmi_csd_ioctl_vs_cmd_voc_4gvwb_set_rate_resp_msg_v01 qmi_csd_4gvwb_set_rate_resp;
		qmi_csd_ioctl_vs_cmd_voc_amr_set_enc_rate_resp_msg_v01 qmi_csd_amr_set_enc_rate_resp;
		qmi_csd_ioctl_vs_cmd_voc_amrwb_set_enc_rate_resp_msg_v01 qmi_csd_amrwb_set_enc_rate_resp;
		qmi_csd_ioctl_vs_cmd_set_dtmf_generation_resp_msg_v01 qmi_csd_dtmf_gen_resp;
		qmi_csd_ioctl_vs_cmd_set_rx_dtmf_detection_resp_msg_v01 qmi_csd_rx_dtmf_dect_resp;
		qmi_csd_ioctl_vs_cmd_set_ui_property_resp_msg_v01 qmi_csd_set_ui_prop_resp;
		qmi_csd_ioctl_vs_cmd_get_ui_property_resp_msg_v01 qmi_csd_get_ui_prop_resp;
		qmi_csd_ioctl_vs_cmd_start_record_resp_msg_v01 qmi_csd_start_record_resp;
		qmi_csd_ioctl_vs_cmd_stop_record_resp_msg_v01 qmi_csd_stop_record_resp;
		qmi_csd_ioctl_vs_cmd_start_playback_resp_msg_v01 qmi_csd_start_playback_resp;
		qmi_csd_ioctl_vs_cmd_stop_playback_resp_msg_v01 qmi_csd_stop_playback_resp;
	}u;
}qmi_csd_vs_cmd_resp_msg;

typedef struct _qmi_csd_vs_cmd_req_msg_v01
{
	union
	{
		qmi_csd_ioctl_vs_cmd_set_media_type_req_msg_v01 qmi_csd_set_media_type_req;
		qmi_csd_ioctl_vs_cmd_set_mute_req_msg_v01 qmi_csd_set_mute_req;
		qmi_csd_ioctl_vs_cmd_set_encoder_dtx_mode_req_msg_v01 qmi_csd_enc_dtx_mode_req;
		qmi_csd_ioctl_vs_cmd_set_dec_timewarp_req_msg_v01 qmi_csd_dec_timewarp_req;
		qmi_csd_ioctl_vs_cmd_set_enc_minmax_rate_req_msg_v01 qmi_csd_enc_minmax_rate_req;
		qmi_csd_ioctl_vs_cmd_set_enc_rate_modulation_req_msg_v01 qmi_csd_enc_rate_modulation_req;
		qmi_csd_ioctl_vs_cmd_voc_qcelp13k_set_rate_req_msg_v01 qmi_csd_qcelp_set_rate_req;
		qmi_csd_ioctl_vs_cmd_voc_4gvnb_set_rate_req_msg_v01 qmi_csd_4gvnb_set_rate_req;
		qmi_csd_ioctl_vs_cmd_voc_4gvwb_set_rate_req_msg_v01 qmi_csd_4gvwb_set_rate_req;
		qmi_csd_ioctl_vs_cmd_voc_amr_set_enc_rate_req_msg_v01 qmi_csd_amr_set_enc_rate_req;
		qmi_csd_ioctl_vs_cmd_voc_amrwb_set_enc_rate_req_msg_v01 qmi_csd_amrwb_set_enc_rate_req;
		qmi_csd_ioctl_vs_cmd_set_dtmf_generation_req_msg_v01 qmi_csd_dtmf_gen_req;
		qmi_csd_ioctl_vs_cmd_set_rx_dtmf_detection_req_msg_v01 qmi_csd_rx_dtmf_dect_req;
		qmi_csd_ioctl_vs_cmd_set_ui_property_req_msg_v01 qmi_csd_set_ui_prop_req;
		qmi_csd_ioctl_vs_cmd_get_ui_property_req_msg_v01 qmi_csd_get_ui_prop_req;
		qmi_csd_ioctl_vs_cmd_start_record_req_msg_v01 qmi_csd_start_record_req;
		qmi_csd_ioctl_vs_cmd_stop_record_req_msg_v01 qmi_csd_stop_record_req;
		qmi_csd_ioctl_vs_cmd_start_playback_req_msg_v01 qmi_csd_start_playback_req;
		qmi_csd_ioctl_vs_cmd_stop_playback_req_msg_v01 qmi_csd_stop_playback_req;
	}u;
}qmi_csd_vs_cmd_req_msg;


uint32_t qmi_csd_vs_callback ( uint32_t cmd_token, uint32_t event_id,
							   void* param, uint32_t param_size, void* cb_data_info);
						 
qmi_csi_cb_error csd_open_passive_control_voice_stream_req_handler 
( client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
);

qmi_csi_cb_error csd_open_full_control_voice_stream_req_handler 
( client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
);

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
  );  
#endif /* __AUDIO_CSD_QMI_SERVICES_VS_H__*/

