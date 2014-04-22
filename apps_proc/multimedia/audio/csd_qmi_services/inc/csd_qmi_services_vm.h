#ifndef __AUDIO_CSD_QMI_SERVICES_VM_H__
#define __AUDIO_CSD_QMI_SERVICES_VM_H__
/*===========================================================================

  A U D I O _ C S D _ Q M I _ S E R V I C E _ V M   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_vm.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/19/12    rp      Added support for voice standby mode
09/06/11    akj     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/

#include "csd_qmi_services_common.h"
#include "csd.h"
#include "csd_vm_ioctl.h"
#include "csd_status.h"
#include "csd_vcommon.h"
#include "csd_debug_msg.h"

/****************************************************************************
   DEFINES
****************************************************************************/


/*===========================================================================
 Function Prototype

===========================================================================*/

typedef struct _qmi_csd_vm_cmd_resp_msg_v01
{
	union
	{
		qmi_csd_ioctl_vm_cmd_attach_stream_resp_msg_v01 qmi_csd_attach_stream_resp;
		qmi_csd_ioctl_vm_cmd_detach_stream_resp_msg_v01 qmi_csd_detach_stream_resp;
		qmi_csd_ioctl_vm_cmd_attach_context_resp_msg_v01 qmi_csd_attach_context_resp;
		qmi_csd_ioctl_vm_cmd_detach_context_resp_msg_v01 qmi_csd_detach_context_resp;
		qmi_csd_ioctl_vm_cmd_start_voice_resp_msg_v01 qmi_csd_start_voice_resp;
		qmi_csd_ioctl_vm_cmd_standby_voice_resp_msg_v01 qmi_csd_standby_voice_resp;
		qmi_csd_ioctl_vm_cmd_pause_voice_resp_msg_v01 qmi_csd_pause_voice_resp;
		qmi_csd_ioctl_vm_cmd_stop_voice_resp_msg_v01 qmi_csd_stop_voice_resp;
		qmi_csd_ioctl_vm_cmd_set_network_resp_msg_v01 qmi_csd_set_network_resp;
		qmi_csd_ioctl_vm_cmd_set_voice_timing_resp_msg_v01 qmi_csd_set_voice_timing_resp;
		qmi_csd_ioctl_vm_cmd_set_tty_mode_resp_msg_v01 qmi_csd_set_tty_mode_resp;
		qmi_csd_ioctl_vm_cmd_set_widevoice_resp_msg_v01 qmi_csd_set_widevoice_resp;
    qmi_csd_ioctl_vm_cmd_set_hdvoice_mode_resp_msg_v01 qmi_csd_set_hdvoice_mode_resp;
	}u;
}qmi_csd_vm_cmd_resp_msg;

typedef struct _qmi_csd_vm_cmd_req_msg_v01
{
	union
	{
		qmi_csd_ioctl_vm_cmd_attach_stream_req_msg_v01 qmi_csd_attach_stream_req;
		qmi_csd_ioctl_vm_cmd_detach_stream_req_msg_v01 qmi_csd_detach_stream_req;
		qmi_csd_ioctl_vm_cmd_attach_context_req_msg_v01 qmi_csd_attach_context_req;
		qmi_csd_ioctl_vm_cmd_detach_context_req_msg_v01 qmi_csd_detach_context_req;
		qmi_csd_ioctl_vm_cmd_start_voice_req_msg_v01 qmi_csd_start_voice_req;
    qmi_csd_ioctl_vm_cmd_standby_voice_req_msg_v01 qmi_csd_standby_voice_req;
		qmi_csd_ioctl_vm_cmd_pause_voice_req_msg_v01 qmi_csd_pause_voice_req;
		qmi_csd_ioctl_vm_cmd_stop_voice_req_msg_v01 qmi_csd_stop_voice_req;
		qmi_csd_ioctl_vm_cmd_set_network_req_msg_v01 qmi_csd_set_network_req;
		qmi_csd_ioctl_vm_cmd_set_voice_timing_req_msg_v01 qmi_csd_set_voice_timing_req;
		qmi_csd_ioctl_vm_cmd_set_tty_mode_req_msg_v01 qmi_csd_set_tty_mode_req;
		qmi_csd_ioctl_vm_cmd_set_widevoice_req_msg_v01 qmi_csd_set_widevoice_req;
    qmi_csd_ioctl_vm_cmd_set_hdvoice_mode_req_msg_v01 qmi_csd_set_hdvoice_mode_req;    
	}u;
}qmi_csd_vm_cmd_req_msg;


uint32_t qmi_csd_vm_callback ( uint32_t cmd_token, uint32_t event_id,
							   void* param, uint32_t param_size, void* cb_data_info);
							   
qmi_csi_cb_error csd_open_voice_manager_req_handler 
( client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
);

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
);

#endif /* __AUDIO_CSD_QMI_SERVICES_VM_H__*/

