#ifndef __AUDIO_CSD_QMI_SERVICES_DEV_H__
#define __AUDIO_CSD_QMI_SERVICES_DEV_H__
/*===========================================================================

 A U D I O _ C S D _ Q M I _ S E R V I C E _ D E V   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_dev.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/13    rp      Added AANC feature support 
12/06/11    jp      Added Device connect command
09/06/11    akj     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/


#include "csd_qmi_services_common.h"
#include "csd.h"
#include "csd_dev_ioctl.h"
#include "csd_debug_msg.h"

/****************************************************************************
   DEFINES
****************************************************************************/


/*===========================================================================
 Function Prototype

===========================================================================*/

typedef struct _qmi_csd_dev_cmd_resp_msg_v01
{
	union
	{
		qmi_csd_ioctl_dev_cmd_enable_resp_msg_v01 qmi_csd_enable_dev_resp;
		qmi_csd_ioctl_dev_cmd_disable_resp_msg_v01 qmi_csd_disable_dev_resp;
		qmi_csd_ioctl_dev_cmd_afe_loopback_resp_msg_v01 qmi_csd_dev_afe_loopback_resp;
		qmi_csd_ioctl_dev_cmd_anc_control_resp_msg_v01 qmi_csd_dev_anc_control_resp;
		qmi_csd_ioctl_dev_cmd_companding_control_resp_msg_v01 qmi_csd_dev_companding_resp;
		qmi_csd_ioctl_dev_cmd_get_max_device_nums_resp_msg_v01 qmi_csd_max_dev_resp;
		qmi_csd_ioctl_dev_cmd_get_dev_caps_resp_msg_v01 qmi_csd_dev_caps_resp;
		qmi_csd_ioctl_dev_cmd_dtmf_control_resp_msg_v01 qmi_csd_dtmf_control_dev_resp;
		qmi_csd_ioctl_dev_cmd_sidetone_control_resp_msg_v01 qmi_csd_dev_sidetone_control_resp;
		qmi_csd_ioctl_dev_cmd_configure_resp_msg_v01 qmi_csd_dev_configure_resp;
		qmi_csd_ioctl_dev_cmd_connect_device_resp_msg_v01 qmi_csd_dev_connect_dev_resp;
                qmi_csd_ioctl_dev_cmd_aanc_control_resp_msg_v01   qmi_csd_dev_aanc_control_resp;
		qmi_csd_ioctl_dev_cmd_restart_resp_msg_v01 qmi_csd_dev_restart_dev_resp;
	}u;
}qmi_csd_dev_cmd_resp_msg;

typedef struct _qmi_csd_dev_cmd_req_msg_v01
{
	union 
	{
		qmi_csd_ioctl_dev_cmd_enable_req_msg_v01 qmi_csd_enable_dev_req;
		qmi_csd_ioctl_dev_cmd_disable_req_msg_v01 qmi_csd_disable_dev_req;
		qmi_csd_ioctl_dev_cmd_afe_loopback_req_msg_v01 qmi_csd_dev_afe_loopback_req;
		qmi_csd_ioctl_dev_cmd_anc_control_req_msg_v01 qmi_csd_dev_anc_control_req;
		qmi_csd_ioctl_dev_cmd_companding_control_req_msg_v01 qmi_csd_dev_companding_req;
		qmi_csd_ioctl_dev_cmd_get_max_device_nums_req_msg_v01 qmi_csd_max_dev_req;
		qmi_csd_ioctl_dev_cmd_get_dev_caps_req_msg_v01 qmi_csd_dev_caps_req;
		qmi_csd_ioctl_dev_cmd_dtmf_control_req_msg_v01 qmi_csd_dtmf_control_dev_req;
		qmi_csd_ioctl_dev_cmd_sidetone_control_req_msg_v01 qmi_csd_dev_sidetone_control_req;
		qmi_csd_ioctl_dev_cmd_configure_req_msg_v01 qmi_csd_dev_configure_req;
		qmi_csd_ioctl_dev_cmd_connect_device_req_msg_v01 qmi_csd_dev_connect_dev_req;
                qmi_csd_ioctl_dev_cmd_aanc_control_req_msg_v01   qmi_csd_dev_aanc_control_req;
		qmi_csd_ioctl_dev_cmd_restart_req_msg_v01 qmi_csd_dev_restart_dev_req;
	}u;
}qmi_csd_dev_cmd_req_msg;


qmi_csi_cb_error csd_open_device_control_req_handler 
( client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
);

qmi_csi_cb_error qmi_csd_ioctl_dev_cmd_req_handler
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
  
#endif /* __AUDIO_CSD_QMI_SERVICES_DEV_H__*/

