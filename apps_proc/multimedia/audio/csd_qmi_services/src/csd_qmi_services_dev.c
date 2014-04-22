/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S _ D E V . C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file for voice context.


Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services_dev.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/14    vk      Added VoUSB feature support
09/01/14    rp      replaced the qmi debug buffer with uniform csd debug buffer
07/18/13    rp      Added AANC feature support 
07/03/13    vk      Changes to remove compiler warnings
06/28/13    rp      Added RT proxy device support
05/24/13    rp      Added Multi device enable support
12/06/11    jp      Added device connect support
09/12/11    akj     Created file (initial version)

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "csd_qmi_services_dev.h"
#include "csd_dev_ioctl.h"
#include "csd_debug_info.h"
#ifdef FEATURE_AUDIO_RTPD_SERVICE
#include "rtpd_status.h"
#include "rtpd_services.h"
#include "rtpd_debug_msg.h"
#endif
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/*=============================================================================
  FUNCTION csd_open_device_control_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_OPEN_DEVICE_CONTROL_REQ
  message is received by the service, sending a response message with the
  device handler.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error csd_open_device_control_req_handler
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
  uint32_t dev_handle;

  qmi_csd_open_device_control_resp_msg_v01 resp;
  memset( &resp, 0, sizeof(qmi_csd_open_device_control_resp_msg_v01) );
  
  dev_handle = csd_open( CSD_OPEN_DEVICE_CONTROL, NULL, 0 );
  if( dev_handle == 0 )
  {
    /* Invalid handle */
    CSD_MSG( CSD_PRIO_ERROR, "csd_open_device_control_req_handler: CSD_OPEN_DEVICE_CONTROL failed, dev_handle[0x%x]", dev_handle );
    
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    resp.open_status_valid = TRUE;
    resp.open_status = (qmi_csd_status_v01) (CSD_EPANIC);
    resp.qmi_csd_device_handle_valid = FALSE;
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_open_device_control_req_handler: CSD_OPEN_DEVICE_CONTROL SUCCESS, dev_handle[0x%x]", dev_handle );
    resp.open_status_valid = FALSE;
    resp.qmi_csd_device_handle_valid = TRUE;
    resp.qmi_csd_device_handle = dev_handle;
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_open_device_control_req_handler: qmi_csi_send_resp failed, rc[0x%x]", resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    QMI_CSD_RESP_SET_BUFFER(msg_id, dev_handle );
    CSD_MSG( CSD_PRIO_HIGH, "csd_open_device_control_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
  }
  return qmi_rc;
}


/*=============================================================================
  FUNCTION qmi_csd_ioctl_vm_cmd_req_handler
=============================================================================*/
/*!
@brief
  This function calls the CSD DEV IOCTL for the QMI message recieved.

@param[in]  handle               CSD DEV handle pertaining to a particular
                                 message
@param[in]  cmd_msg_id           CSD IOCTL value for the perticular 
                                 message from the client
@param[in]  clnt_info            connection handle used by infrastructure
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
)
{
  uint32_t csd_rc = CSD_EOK;
#ifdef FEATURE_AUDIO_RTPD_SERVICE
  uint32_t rtpd_rc = RTPD_EOK;
#endif
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;
  uint32_t hdl = handle;
  uint32_t resp_msg_size = 0;

  qmi_csd_dev_cmd_req_msg *qmi_csd_dev_req = (qmi_csd_dev_cmd_req_msg*)req_c_struct;
  qmi_csd_dev_cmd_resp_msg *resp_msg;
  resp_msg = ( qmi_csd_dev_cmd_resp_msg*)malloc( sizeof(qmi_csd_dev_cmd_resp_msg) );
  
  if( resp_msg != NULL ) 
  {
    memset( resp_msg, 0, sizeof(qmi_csd_dev_cmd_resp_msg) );
    
    switch( cmd_msg_id ) 
    {
      case CSD_DEV_CMD_CONFIGURE:
      {
        csd_dev_configure_t dev_configure;
        qmi_csd_ioctl_dev_cmd_configure_req_msg_v01*  qmi_dev_config;
        csd_dev_rt_proxy_port_cfg_t *rt_port_cfg = NULL;
		
#ifdef FEATURE_AUDIO_RTPD_SERVICE	
        rtpd_dev_configure_t rtpd_dev_configure_info;
        rtpd_dev_rt_proxy_port_cfg_t *rt_port_cfg_data = NULL;
#endif
		
        qmi_dev_config = (qmi_csd_ioctl_dev_cmd_configure_req_msg_v01*)
                           ( &(qmi_csd_dev_req->u.qmi_csd_dev_configure_req));
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONFIGURE called,"
                        "dev id [0x%x] to configure", qmi_dev_config->dev_id );
        /*conifuring only one device as QMI RT proxy configure command 
          supports only one device at a time */
        rt_port_cfg = &dev_configure.devs[0].csd_dev_cfg.rt_proxy_port_cfg;
        dev_configure.num_devs = 1;
        dev_configure.devs[0].dev_id = qmi_dev_config->dev_id;
        rt_port_cfg->cfg_hdr  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.cfg_hdr;
        rt_port_cfg->num_channels  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.num_channels;
        rt_port_cfg->interleaved  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.interleaved;
        rt_port_cfg->frame_size  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.frame_size;
        rt_port_cfg->jitter_allowance  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.jitter_allowance;
        rt_port_cfg->low_water_mark  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.low_water_mark;
        rt_port_cfg->high_water_mark  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.high_water_mark;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: rt proxy device cfg hdr :0x%x  num_channels = 0x%x"
                                " interleaved = 0x%x  frame_size = 0x%x",
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.cfg_hdr,
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.num_channels,
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.interleaved,
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.frame_size);
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: rjitter_allowance :0x%x  low_water_mark = 0x%x"
                                "  high_water_mark = 0x%x ",
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.jitter_allowance,
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.low_water_mark,
                 qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.high_water_mark);

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_CONFIGURE, &dev_configure, sizeof(csd_dev_configure_t));

#ifdef FEATURE_AUDIO_RTPD_SERVICE

        rt_port_cfg_data = &rtpd_dev_configure_info.devs[0].rtpd_dev_cfg.rt_proxy_port_cfg;
        rtpd_dev_configure_info.num_devs = 1;
        rtpd_dev_configure_info.devs[0].dev_id = qmi_dev_config->dev_id;
        rt_port_cfg_data->cfg_hdr  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.cfg_hdr;
        rt_port_cfg_data->num_channels  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.num_channels;
        rt_port_cfg_data->interleaved  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.interleaved;
        rt_port_cfg_data->frame_size  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.frame_size;
        rt_port_cfg_data->jitter_allowance  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.jitter_allowance;
        rt_port_cfg_data->low_water_mark  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.low_water_mark;
        rt_port_cfg_data->high_water_mark  =
              qmi_dev_config->qmi_csd_dev_rt_proxy_port_cfg_payload.high_water_mark;
		  
        rtpd_rc = rtpd_service_dev_cmd_configure(&rtpd_dev_configure_info, sizeof(rtpd_dev_configure_t));
        if( rtpd_rc != RTPD_EOK )
        {
          RTPD_MSG( RTPD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: RTPD_DEV_CMD_CONFIGURE failed, rtpd_rc[0x%x]", rtpd_rc );
        }
        else
        {
          RTPD_MSG( RTPD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: RTPD_DEV_CMD_CONFIGURE success, rtpd_rc[0x%x]", rtpd_rc );
        }

#endif
        if (csd_rc != CSD_EOK)
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONFIGURE failed, csd_rc[0x%x]", csd_rc );

          resp_msg->u.qmi_csd_dev_configure_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_configure_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_configure_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dev_configure_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONFIGURE success" );
          resp_msg->u.qmi_csd_dev_configure_resp.qmi_csd_status_code_valid = FALSE;
        }

        resp_msg_size = sizeof(resp_msg->u.qmi_csd_enable_dev_resp);

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONFIGURE called" );
        break;
      }
      
      case CSD_DEV_CMD_ENABLE:
      {
        csd_dev_enable_t qmi_csd_dev_enable_info;
#ifdef FEATURE_AUDIO_RTPD_SERVICE
        rtpd_dev_enable_t rtpd_dev_enable_info;
#endif
        qmi_csd_dev_entry_v01*   extn_devices;
        qmi_csd_dev_enable_v01 *req_dev_en = (qmi_csd_dev_enable_v01 *)( &(qmi_csd_dev_req->u.qmi_csd_enable_dev_req.qmi_csd_dev_enable_cmd_payload) );
        uint32_t  i,j;
        
        qmi_csd_dev_enable_info.num_devs = req_dev_en->devs_len;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_ENABLE called, num of dev[%d] to enable", req_dev_en->devs_len );
        for( i=0; i< qmi_csd_dev_enable_info.num_devs; i++ )
        {
          qmi_csd_dev_enable_info.devs[i].dev_id = req_dev_en->devs[i].dev_id;
          qmi_csd_dev_enable_info.devs[i].dev_attrib.sample_rate = req_dev_en->devs[i].dev_attrib.sample_rate;
          qmi_csd_dev_enable_info.devs[i].dev_attrib.bits_per_sample = req_dev_en->devs[i].dev_attrib.bits_per_sample;
          
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: dev_id[0x%x] sr[%d] bps[%d]", req_dev_en->devs[i].dev_id,
                   req_dev_en->devs[i].dev_attrib.sample_rate, req_dev_en->devs[i].dev_attrib.bits_per_sample );
        }
        /*Enabling the extended devices */
        if (qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices_valid)
        {
           extn_devices = (qmi_csd_dev_entry_v01*)(&(qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices));
           /* Check the extended devices length*/
           CSD_MSG(CSD_PRIO_MEDIUM,"qmi_csd_ioctl_dev_cmd_req_handler: Number of extended devices num of dev[%d]",
                       qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices_len);
           if (qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices_len <= QMI_CSD_EXTENDED_DEVICES_MAX_ENTRIES_V01)
           {
              qmi_csd_dev_enable_info.num_devs += qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices_len;
              /*initializing with normal devices length*/
              j = req_dev_en->devs_len;
              for (i=0; i< qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices_len; i++,j++)
              {
                qmi_csd_dev_enable_info.devs[j].dev_id = extn_devices[i].dev_id;
                qmi_csd_dev_enable_info.devs[j].dev_attrib.sample_rate = extn_devices[i].dev_attrib.sample_rate;
                qmi_csd_dev_enable_info.devs[j].dev_attrib.bits_per_sample = extn_devices[i].dev_attrib.bits_per_sample;
                CSD_MSG(CSD_PRIO_HIGH,"qmi_csd_ioctl_dev_cmd_req_handler: dev_id[0x%x] sr[%d] bps[%d] ",extn_devices[i].dev_id,
                             extn_devices[i].dev_attrib.sample_rate, extn_devices[i].dev_attrib.bits_per_sample);
              }
           }
           else
           {
             CSD_MSG(CSD_PRIO_ERROR,"qmi_csd_ioctl_dev_cmd_req_handler: Invalid Number of extended devices num of dev[%d]",
                         qmi_csd_dev_req->u.qmi_csd_enable_dev_req.extn_devices_len);
           }
        }

#ifdef FEATURE_AUDIO_RTPD_SERVICE

        rtpd_dev_enable_info.num_devs = qmi_csd_dev_enable_info.num_devs;
        for ( i=0; i<rtpd_dev_enable_info.num_devs; i++)
        {
          rtpd_dev_enable_info.devs[i].dev_id = qmi_csd_dev_enable_info.devs[i].dev_id;
          rtpd_dev_enable_info.devs[i].dev_attrib.sample_rate = qmi_csd_dev_enable_info.devs[i].dev_attrib.sample_rate;
          rtpd_dev_enable_info.devs[i].dev_attrib.bits_per_sample = qmi_csd_dev_enable_info.devs[i].dev_attrib.bits_per_sample;
        }

        rtpd_rc = rtpd_service_device_enable( &rtpd_dev_enable_info, sizeof(rtpd_dev_enable_t));
        if(rtpd_rc != RTPD_EOK)
        {
          RTPD_MSG( RTPD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: RTPD_DEV_CMD_ENABLE failed, rtpd_rc[0x%x]", rtpd_rc );
        }
        else
        {
          RTPD_MSG( RTPD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: RTPD_DEV_CMD_ENABLE success, rtpd_rc[0x%x]", rtpd_rc );
        }
#endif

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_ENABLE, &qmi_csd_dev_enable_info, sizeof(csd_dev_enable_t) ); 

        if(csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_ENABLE failed, csd_rc[0x%x]", csd_rc );

          resp_msg->u.qmi_csd_enable_dev_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_enable_dev_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_enable_dev_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_enable_dev_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_ENABLE success" );
          resp_msg->u.qmi_csd_enable_dev_resp.qmi_csd_status_code_valid = FALSE;
        }

        resp_msg_size = sizeof(resp_msg->u.qmi_csd_enable_dev_resp);
      }
      break;
    
      case CSD_DEV_CMD_DISABLE:
      {
        csd_dev_disable_t qmi_csd_dev_disable_info;
#ifdef FEATURE_AUDIO_RTPD_SERVICE
        rtpd_dev_disable_t rtpd_dev_disable_info;
#endif
        uint32_t* extn_devices_ids;
        qmi_csd_dev_disable_v01 *req_dev_ids = (qmi_csd_dev_disable_v01 *)( &(qmi_csd_dev_req->u.qmi_csd_disable_dev_req.qmi_csd_dev_disable_cmd_payload) );
        uint32_t  i,j;

        qmi_csd_dev_disable_info.num_devs = req_dev_ids->dev_ids_len;
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_DISABLE called, num of dev[%d] to disable", req_dev_ids->dev_ids_len);
        
        for( i=0; i< qmi_csd_dev_disable_info.num_devs; i++ )
        {
          qmi_csd_dev_disable_info.dev_ids[i] = req_dev_ids->dev_ids[i];
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: dev_id[0x%x] to disable", req_dev_ids->dev_ids[i] );
        }
        /*Disable the extended devices */
        if (qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices_valid)
        {
          extn_devices_ids = (uint32_t*) (&(qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices));
          CSD_MSG(CSD_PRIO_HIGH,"qmi_csd_ioctl_dev_cmd_req_handler: Extended device length[%d]",
                         qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices_len);
          if (qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices_len <= QMI_CSD_EXTENDED_DEVICES_MAX_ENTRIES_V01)
          {
            qmi_csd_dev_disable_info.num_devs += qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices_len;
            /*initializing with normal devices length*/
            j = req_dev_ids->dev_ids_len;
            for (i=0;i<qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices_len;i++,j++)
            {
              qmi_csd_dev_disable_info.dev_ids[j] = extn_devices_ids[i];
              CSD_MSG(CSD_PRIO_HIGH,"qmi_csd_ioctl_dev_cmd_req_handler: Dev_id[0x%x]", extn_devices_ids[i]);
            }
          }
          else
          {
            CSD_MSG(CSD_PRIO_ERROR,"qmi_csd_ioctl_dev_cmd_req_handler: Invalid Number of extended devices num of dev[%d]",
                          qmi_csd_dev_req->u.qmi_csd_disable_dev_req.extn_devices_len);
          }
        }

#ifdef FEATURE_AUDIO_RTPD_SERVICE

        rtpd_dev_disable_info.num_devs = qmi_csd_dev_disable_info.num_devs;
        for ( i=0; i<rtpd_dev_disable_info.num_devs; i++)
        {
          rtpd_dev_disable_info.dev_ids[i] = qmi_csd_dev_disable_info.dev_ids[i];
        }
#endif
        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_DISABLE, &qmi_csd_dev_disable_info, sizeof(csd_dev_disable_t) );

#ifdef FEATURE_AUDIO_RTPD_SERVICE

        rtpd_rc = rtpd_service_device_disable( &rtpd_dev_disable_info, sizeof(rtpd_dev_disable_t));
        if (rtpd_rc != RTPD_EOK)
        {
          RTPD_MSG( RTPD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: RTPD_DEV_CMD_DISABLE failed, rtpd_rc[0x%x]", rtpd_rc );
        }
        else
        {
          RTPD_MSG( RTPD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: RTPD_DEV_CMD_ENABLE success, rtpd_rc[0x%x]", rtpd_rc );
        }

#endif
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_DISABLE failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_disable_dev_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_disable_dev_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_disable_dev_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_disable_dev_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_DISABLE success" );
          resp_msg->u.qmi_csd_disable_dev_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_disable_dev_resp);
      }
      break;
    
      case CSD_DEV_CMD_GET_MAX_DEV_NUMS:
      {
        csd_dev_max_dev_nums_t qmi_csd_dev_max_info;

        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_GET_MAX_DEV_NUMS called" );

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_GET_MAX_DEV_NUMS, &qmi_csd_dev_max_info, sizeof(qmi_csd_dev_max_info) );

        if( csd_rc != CSD_EOK ) 
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_GET_MAX_DEV_NUMS failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_max_dev_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_max_dev_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_max_dev_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_max_dev_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
          resp_msg->u.qmi_csd_max_dev_resp.max_num_devices_valid = FALSE;
        }
        else 
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_GET_MAX_DEV_NUMS success, max_dev[%d]", qmi_csd_dev_max_info.max_num_devs );
          resp_msg->u.qmi_csd_max_dev_resp.qmi_csd_status_code_valid = FALSE;
          resp_msg->u.qmi_csd_max_dev_resp.max_num_devices_valid = TRUE;
          resp_msg->u.qmi_csd_max_dev_resp.max_num_devices = qmi_csd_dev_max_info.max_num_devs;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_max_dev_resp);
      }
      break;
    
      case CSD_DEV_CMD_AFE_LOOPBACK:
      {
        csd_dev_afe_loopback_t qmi_csd_afe_loopback_info;
        qmi_csd_dev_afe_loopback_v01 *req_dev_afe_loopback = (qmi_csd_dev_afe_loopback_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dev_afe_loopback_req.qmi_csd_dev_afe_loopback_cmd_payload));

        qmi_csd_afe_loopback_info.afe_mode  = req_dev_afe_loopback->afe_mode;
        qmi_csd_afe_loopback_info.enable    = req_dev_afe_loopback->enable;
        qmi_csd_afe_loopback_info.rx_dev_id = req_dev_afe_loopback->rx_dev_id;
        qmi_csd_afe_loopback_info.tx_dev_id = req_dev_afe_loopback->tx_dev_id;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_AFE_LOOPBACK called, afe_mode[0x%x] enable[0x%x] rx_dev[0x%x] tx_dev[0x%x]", req_dev_afe_loopback->afe_mode, 
                 req_dev_afe_loopback->enable, req_dev_afe_loopback->rx_dev_id, req_dev_afe_loopback->tx_dev_id );

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_AFE_LOOPBACK, &qmi_csd_afe_loopback_info, sizeof(csd_dev_afe_loopback_t) );

        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_AFE_LOOPBACK failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_dev_afe_loopback_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_afe_loopback_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_afe_loopback_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dev_afe_loopback_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_AFE_LOOPBACK success" );
          
          resp_msg->u.qmi_csd_dev_afe_loopback_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_afe_loopback_resp);
      }
      break;
    
      case CSD_DEV_CMD_CONNECT_DEVICE:
      {
        csd_dev_connect_device_t qmi_csd_dev_connect_info;
        qmi_csd_dev_connect_device_v01 *req_dev_connect = (qmi_csd_dev_connect_device_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dev_connect_dev_req.qmi_csd_dev_connect_device_cmd_payload));
        csd_dev_pair_t pair_info;
        
        pair_info.source_dev_id = req_dev_connect->source_dev_id;
        pair_info.sink_dev_id = req_dev_connect->sink_dev_id;

        qmi_csd_dev_connect_info.pairs = NULL;
        qmi_csd_dev_connect_info.num_pairs  = 1;
        qmi_csd_dev_connect_info.connect_flag = req_dev_connect->connect_flag;
        qmi_csd_dev_connect_info.pairs = &pair_info;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONNECT_DEVICE called, num_pair[%d] connect_flag[0x%x] src_dev_id[0x%x] sink_dev_id[0x%x]", qmi_csd_dev_connect_info.num_pairs, 
                 req_dev_connect->connect_flag, req_dev_connect->source_dev_id, req_dev_connect->sink_dev_id );

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_CONNECT_DEVICE, &qmi_csd_dev_connect_info, sizeof(csd_dev_connect_device_t) );

        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONNECT_DEVICE failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_dev_connect_dev_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_connect_dev_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_connect_dev_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dev_connect_dev_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_CONNECT_DEVICE success" );
          
          resp_msg->u.qmi_csd_dev_connect_dev_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_connect_dev_resp);
      }
      break;
    
      case CSD_DEV_CMD_ANC_CONTROL:
      {
        csd_cmd_dev_anc_t qmi_csd_dev_anc_info;
        qmi_csd_dev_anc_v01 *req_dev_anc = (qmi_csd_dev_anc_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dev_anc_control_req.qmi_csd_dev_anc_cmd_payload));

        qmi_csd_dev_anc_info.enable    = req_dev_anc->enable;
        qmi_csd_dev_anc_info.rx_dev_id = req_dev_anc->rx_dev_id;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_ANC_CONTROL called, enable[%d] rx_dev[0x%x]", req_dev_anc->enable, req_dev_anc->rx_dev_id );

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_ANC_CONTROL, &qmi_csd_dev_anc_info, sizeof(csd_cmd_dev_anc_t) );

        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_ANC_CONTROL failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_dev_anc_control_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_anc_control_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_anc_control_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dev_anc_control_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_ANC_CONTROL success" );
          
          resp_msg->u.qmi_csd_dev_anc_control_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_anc_control_resp);
      }
      break;
    
      case CSD_DEV_CMD_COMPANDING_CONTROL:
      {
        csd_dev_companding_t qmi_csd_dev_companding_info;
        qmi_csd_dev_companding_v01 *req_dev_companding = (qmi_csd_dev_companding_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dev_companding_req.qmi_csd_dev_companding_cmd_payload));
    
        qmi_csd_dev_companding_info.comp_options = (uint32_t)req_dev_companding->qmi_csd_comp_options;
        qmi_csd_dev_companding_info.rx_dev_id    = req_dev_companding->rx_dev_id;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_COMPANDING_CONTROL called, comp_option[0x%x] rx_dev_id[0x%x]",
                 req_dev_companding->qmi_csd_comp_options, req_dev_companding->rx_dev_id );
    
        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_COMPANDING_CONTROL, &qmi_csd_dev_companding_info, sizeof(csd_dev_companding_t) );
    
        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_COMPANDING_CONTROL failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_dev_companding_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_companding_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_companding_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dev_companding_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_COMPANDING_CONTROL success" );
          resp_msg->u.qmi_csd_dev_companding_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_companding_resp);
      }
      break;
    
      case CSD_DEV_CMD_DTMF_CONTROL:
      {
        csd_dev_dtmf_ctrl_t qmi_csd_dev_dtmf_cntl_info;
        qmi_csd_dev_dtmf_ctrl_v01 *req_dev_dtmf_cntl = (qmi_csd_dev_dtmf_ctrl_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dtmf_control_dev_req.qmi_csd_dev_dtmf_cmd_payload));
        uint32_t  i;
        
        qmi_csd_dev_dtmf_cntl_info.dtmf_duration_in_ms = req_dev_dtmf_cntl->dtmf_duration_in_ms;
        qmi_csd_dev_dtmf_cntl_info.dtmf_gain           = req_dev_dtmf_cntl->dtmf_gain;
        qmi_csd_dev_dtmf_cntl_info.dtmf_high_freq      = req_dev_dtmf_cntl->dtmf_high_freq;
        qmi_csd_dev_dtmf_cntl_info.dtmf_low_freq       = req_dev_dtmf_cntl->dtmf_low_freq;
        qmi_csd_dev_dtmf_cntl_info.num_devs            = req_dev_dtmf_cntl->dev_ids_len;

        qmi_csd_dev_dtmf_cntl_info.p_dev_ids = (uint32_t*)malloc((sizeof(uint32_t))* qmi_csd_dev_dtmf_cntl_info.num_devs);
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_DTMF_CONTROL called, duration[%lld], gain[0x%x], hi_freq[%d], low_freq[%d], num_devs[%d]",
                 req_dev_dtmf_cntl->dtmf_duration_in_ms, req_dev_dtmf_cntl->dtmf_gain, req_dev_dtmf_cntl->dtmf_high_freq,
                 req_dev_dtmf_cntl->dtmf_low_freq, req_dev_dtmf_cntl->dev_ids_len );

        if( qmi_csd_dev_dtmf_cntl_info.p_dev_ids == NULL )
        {
          csd_rc = CSD_EFAILED;
        }
        else
        {
          for( i=0; i< qmi_csd_dev_dtmf_cntl_info.num_devs; i++ ) 
          {
            *((qmi_csd_dev_dtmf_cntl_info.p_dev_ids)+i) = req_dev_dtmf_cntl->dev_ids[i];
          }

          csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_DTMF_CONTROL, &qmi_csd_dev_dtmf_cntl_info, sizeof(csd_dev_dtmf_ctrl_t) );
        }

        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_DTMF_CONTROL failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_dtmf_control_dev_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dtmf_control_dev_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dtmf_control_dev_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dtmf_control_dev_resp.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_DTMF_CONTROL success" );
          resp_msg->u.qmi_csd_dtmf_control_dev_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dtmf_control_dev_resp);
        free(qmi_csd_dev_dtmf_cntl_info.p_dev_ids);
      }
      break;
    
      case CSD_DEV_CMD_SIDETONE_CONTROL:
      {
        csd_dev_sidetone_ctrl_t qmi_csd_dev_sidetone_info;
        qmi_csd_dev_sidetone_ctrl_v01 *req_dev_sidetone = (qmi_csd_dev_sidetone_ctrl_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dev_sidetone_control_req.qmi_csd_dev_sidetone_cmd_payload));

        qmi_csd_dev_sidetone_info.ctrl      = (uint32_t)req_dev_sidetone->sidetone_ctrl;
        qmi_csd_dev_sidetone_info.rx_dev_id = req_dev_sidetone->rx_dev_id;
        qmi_csd_dev_sidetone_info.tx_dev_id = req_dev_sidetone->tx_dev_id;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_SIDETONE_CONTROL called, sidetone_ctrl[%d] rx_dev[0x%x] tx_dev[0x%x]",
                 req_dev_sidetone->sidetone_ctrl, req_dev_sidetone->rx_dev_id, req_dev_sidetone->tx_dev_id );

        csd_rc = csd_ioctl( hdl, CSD_DEV_CMD_SIDETONE_CONTROL, &qmi_csd_dev_sidetone_info, sizeof(csd_dev_sidetone_ctrl_t) );

        if( csd_rc != CSD_EOK )
        {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_SIDETONE_CONTROL failed, csd_rc[0x%x]", csd_rc );
          
          resp_msg->u.qmi_csd_dev_sidetone_control_resp.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_sidetone_control_resp.resp.error = QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_sidetone_control_resp.qmi_csd_status_code_valid = TRUE;
          resp_msg->u.qmi_csd_dev_sidetone_control_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
        }
        else
        {
          CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_SIDETONE_CONTROL success" );
          
          resp_msg->u.qmi_csd_dev_sidetone_control_resp.qmi_csd_status_code_valid = FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_sidetone_control_resp);
      }
      break;
      
      case CSD_DEV_CMD_AANC_CONTROL:
      {
        csd_cmd_dev_aanc_t   qmi_csd_dev_aanc_ctl;
        qmi_csd_ioctl_dev_cmd_aanc_control_req_msg_v01 * qmi_aanc_msg_req =
                       (&(qmi_csd_dev_req->u.qmi_csd_dev_aanc_control_req));
        CSD_MSG(CSD_PRIO_HIGH,"qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_AANC_CONTROL called "
                             "aanc_ctrl:[%d]",qmi_aanc_msg_req->aanc_ctrl);
        switch (qmi_aanc_msg_req->aanc_ctrl)
        {
          case QMI_CSD_DEV_AFE_AANC_DISABLE_V01:
            qmi_csd_dev_aanc_ctl.aanc_ctrl = CSD_DEV_AFE_AANC_DISABLE;
            break;
          case QMI_CSD_DEV_AFE_AANC_ENABLE_V01:
            qmi_csd_dev_aanc_ctl.aanc_ctrl = CSD_DEV_AFE_AANC_ENABLE;
            break;
          case QMI_CSD_DEV_AFE_AANC_ACDB_CTRL_V01:
            qmi_csd_dev_aanc_ctl.aanc_ctrl = CSD_DEV_AFE_AANC_DEFAULT;
            break;
          default:
            csd_rc = CSD_EFAILED;
            CSD_MSG(CSD_PRIO_ERROR,"qmi_csd_ioctl_dev_cmd_req_handler: Invalid CSD_DEV_CMD_AANC_CONTROL [%d]",
                    qmi_aanc_msg_req->aanc_ctrl);
            break;
        }
        if( csd_rc == CSD_EOK )
        {
          qmi_csd_dev_aanc_ctl.tx_dev_id   = qmi_aanc_msg_req->tx_dev_id;
          qmi_csd_dev_aanc_ctl.rx_dev_id   = qmi_aanc_msg_req->rx_dev_id;
          qmi_csd_dev_aanc_ctl.ref_dev_id  = qmi_aanc_msg_req->ref_dev_id;
          CSD_MSG(CSD_PRIO_HIGH,"qmi_csd_ioctl_dev_cmd_req_handler: AANC control[%d] tx_dev[0x%x]"
                                "rx_dev[0x%x] ref_dev[0x%x]",
                  qmi_aanc_msg_req->aanc_ctrl, qmi_aanc_msg_req->tx_dev_id,
                  qmi_aanc_msg_req->rx_dev_id, qmi_aanc_msg_req->ref_dev_id);

          csd_rc = csd_ioctl (hdl, CSD_DEV_CMD_AANC_CONTROL,
                        &qmi_csd_dev_aanc_ctl, sizeof(qmi_csd_dev_aanc_ctl));
        }

        if ((csd_rc != CSD_EOK))
        {
          CSD_MSG(CSD_PRIO_ERROR,"qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_AANC_CONTROL FAILED "
                                       "csd_rc[0x%x]", csd_rc);
          resp_msg->u.qmi_csd_dev_aanc_control_resp.resp.result = 
                                                   QMI_RESULT_FAILURE_V01;
          resp_msg->u.qmi_csd_dev_aanc_control_resp.resp.error = 
                                                   QMI_ERR_GENERAL_V01;
          resp_msg->u.qmi_csd_dev_aanc_control_resp.qmi_csd_status_code_valid = 
                                                   TRUE;
          resp_msg->u.qmi_csd_dev_aanc_control_resp.qmi_csd_status_code = 
                                              (qmi_csd_status_v01) csd_rc;
        }
        else
        {
          /* success */
          CSD_MSG(CSD_PRIO_MEDIUM,"qmi_csd_ioctl_dev_cmd_req_handler: CSD_DEV_CMD_AANC_CONTROL success");
          resp_msg->u.qmi_csd_dev_aanc_control_resp.qmi_csd_status_code_valid =
                                                                   FALSE;
        }
        resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_aanc_control_resp);
      }
      break;

    case CSD_DEV_CMD_RESTART:
    {
      csd_cmd_dev_restart_t qmi_csd_dev_restart;
      qmi_csd_dev_restart_v01 *req_dev_restart = (qmi_csd_dev_restart_v01 *)(&(qmi_csd_dev_req->u.qmi_csd_dev_restart_dev_req.qmi_csd_dev_restart_payload));
  
      CSD_MSG(CSD_PRIO_MEDIUM,"CSD_DEV_CMD_RESTART \n");
  
      qmi_csd_dev_restart.rx_dev_id = (uint32_t)req_dev_restart->rx_dev_id;
      qmi_csd_dev_restart.tx_dev_id = (uint32_t)req_dev_restart->tx_dev_id;
      qmi_csd_dev_restart.sample_rate = (uint32_t)req_dev_restart->sample_rate;
      CSD_MSG(CSD_PRIO_MEDIUM,"restart rx_dev[%08X] tx_dev[%08X] sr [%d] \n",
                 req_dev_restart->rx_dev_id, req_dev_restart->tx_dev_id, req_dev_restart->sample_rate);
  
      csd_rc = csd_ioctl (hdl, CSD_DEV_CMD_RESTART, &qmi_csd_dev_restart, sizeof(csd_cmd_dev_restart_t));
  
      memset( resp_msg, 0, sizeof(qmi_csd_dev_cmd_resp_msg));
  
      if ((csd_rc != CSD_EOK))
      {
        CSD_MSG(CSD_PRIO_ERROR,"CSD_DEV_CMD_RESTART failed, csd_rc = 0x%x\n", csd_rc);
        resp_msg->u.qmi_csd_dev_restart_dev_resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg->u.qmi_csd_dev_restart_dev_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg->u.qmi_csd_dev_restart_dev_resp.qmi_csd_status_code_valid = TRUE;
        resp_msg->u.qmi_csd_dev_restart_dev_resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
      }
      else
      {
        // succeed
        CSD_MSG(CSD_PRIO_MEDIUM,"CSD_DEV_CMD_RESTART success");
        resp_msg->u.qmi_csd_dev_restart_dev_resp.qmi_csd_status_code_valid = FALSE;
      }
      resp_msg_size = sizeof(resp_msg->u.qmi_csd_dev_restart_dev_resp);
    }
    break;

      default:
        csd_rc = CSD_EFAILED;
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: Unknown cmd_msg_id[0x%x]", csd_rc );
      break;
    }

    resp_err = qmi_csi_send_resp( req_handle, msg_id, resp_msg, resp_msg_size );
    if( resp_err != QMI_CSI_NO_ERR )
    {
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_dev_cmd_req_handler: qmi_csi_send_resp failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
    }
    else
    {
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csd_ioctl_dev_cmd_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
      qmi_rc = QMI_CSI_CB_NO_ERR;
      QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc );
    }
    free( resp_msg );
  }
  return qmi_rc;
}
