/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S _ L I T E . C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file for CSDLite module.


Copyright (c) 2015-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: 
$DateTime: 

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/07/15    jy     initial version

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "csd_qmi_services_lite.h"
#include "csd_qmi_services_common.h"
#include "csd_debug_info.h"
#include "csd_os_dependencies.h"
#include "csdlite.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
static client_info_type *client_info; // This works with the assumption of single client


uint32_t qmi_csdlite_callback(
  uint32_t cmd_token, 
  uint32_t event_id,
  void* param, 
  uint32_t param_size, 
  void* cb_data_info )
{
  qmi_csi_error resp_err;
  
  switch( event_id ) {
  
    case CSDLITE_RX_DTMF_DETECTED:
    case CSDLITE_TX_DTMF_DETECTED: {
      qmi_csd_voice_dtmf_detected_ind_msg_v01 ind;    
      csdlite_dtmf_event_t *event = (csdlite_dtmf_event_t*)param;
     
      ind.low_freq = event->low_freq;
      ind.high_freq = event->high_freq;
      if( event_id == CSDLITE_RX_DTMF_DETECTED )
        ind.direction = CSD_VOICE_RX_DTMF_DETECTED_V01;
      else
        ind.direction = CSD_VOICE_TX_DTMF_DETECTED_V01;

      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_callback: DTMF tone detected, high_freq[%d], low_freq[%d], direction[%d]", 
               ind.high_freq, ind.low_freq, ind.direction );

      if( !client_info ) {
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_callback: client info is NULL, unable to send the indication" );
        break;
      }
               
      resp_err = qmi_csi_send_ind( client_info->clnt, QMI_CSD_VOICE_DTMF_DETECTED_IND_V01, &ind, sizeof(ind) );
      if( resp_err != QMI_CSI_NO_ERR )
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_callback: qmi_csi_send_ind failed for QMI_CSD_VOICE_DTMF_DETECTED_IND_V01, rc[0x%x]", resp_err );
    }
    break;
    
    case CSDLITE_DTMF_GENERATION_ENDED: {
      qmi_csd_voice_dtmf_generation_ended_ind_msg_v01 ind;
      
      ind.direction = CSD_DTMF_GENERATION_DIR_TX_V01; // Only Tx is supported
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_callback: DTMF tone ended, direction[%d]", ind.direction );

      if( !client_info ) {
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_callback: client info is NULL, unable to send the indication" );
        break;
      }
        
      resp_err = qmi_csi_send_ind( client_info->clnt, QMI_CSD_VOICE_DTMF_GENERATION_ENDED_IND_V01, &ind, sizeof(ind) );
      if( resp_err != QMI_CSI_NO_ERR )
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_callback: qmi_csi_send_ind failed for QMI_CSD_VOICE_DTMF_GENERATION_ENDED_IND_V01, rc[0x%x]", resp_err );
    }
    break;
  }
  
  return 0;
}                         
    

/*=============================================================================
  FUNCTION qmi_csdlite_handle_req
=============================================================================*/
/*!
@brief
  This function handles all new QMI_CSD msgs introduced after version 1.12

  PARAMETERS
    @param[in]  clnt_info          Information provided by the client
    @param[in]  req_handle         Opaque handle provided by the infrastructure
                                   to specify a particular transaction and
                                   message.
    @param[in]  msg_id             Message Id pertaining to a particular
                                   message.
    @param[in]  req_c_struct       C struct with the decoded data.
    @param[in]  req_c_struct_len   Length of the c struct.
    @param[in]  service_cookie     Service specific data. Service cookie is
                                   registered with the infrastructure during
                                   service registration(qmi_csi_register).

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
qmi_csi_cb_error qmi_csdlite_handle_req
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
  uint32_t csd_rc = CSD_EFAILED;
  qmi_csi_error resp_err;    
  qmi_csd_common_resp_msg_v01 resp_msg;
  qmi_error_type_v01 error = QMI_ERR_NONE_V01;
  
  client_info = clnt_info;
  
  switch( msg_id ) {
  
    case QMI_CSD_VOICE_CONFIG_REQ_V01: {
      qmi_csd_voice_config_req_msg_v01 *req_info = (qmi_csd_voice_config_req_msg_v01 *)req_c_struct;
      char_t session_id[SESSION_NAME_MAX_LEN_V01 + 1];
      csdlite_voice_config_t config;
      
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_CONFIG_REQ_V01 called" );
      
      strlcpy( session_id, req_info->session_id, sizeof(req_info->session_id) );
      config.session_id = session_id;
      config.cmd_token = req_info->cmd_token;
      config.dev_info.tx_dev.dev_id = 0xFFFFFFFF;
      config.dev_info.rx_dev.dev_id = 0xFFFFFFFF;
      config.dev_info.ec_ref_dev.dev_id = 0xFFFFFFFF;
      
      if( req_info->tx_device_info_valid ) {
        config.dev_info.tx_dev.dev_id = req_info->tx_device_info.dev_id;
        config.dev_info.tx_dev.sample_rate = req_info->tx_device_info.sample_rate;
        config.dev_info.tx_dev.bits_per_sample = req_info->tx_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "tx - id[%d], sample_rate[%d], bps[%d]", config.dev_info.tx_dev.dev_id, 
                 config.dev_info.tx_dev.sample_rate, config.dev_info.tx_dev.bits_per_sample );
      }
      if( req_info->rx_device_info_valid ) {
        config.dev_info.rx_dev.dev_id = req_info->rx_device_info.dev_id;
        config.dev_info.rx_dev.sample_rate = req_info->rx_device_info.sample_rate;
        config.dev_info.rx_dev.bits_per_sample = req_info->rx_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "rx - id[%d], sample_rate[%d], bps[%d]", config.dev_info.rx_dev.dev_id, 
                 config.dev_info.rx_dev.sample_rate, config.dev_info.rx_dev.bits_per_sample );
      }
      if( req_info->ec_ref_device_info_valid ) {
        config.dev_info.ec_ref_dev.dev_id = req_info->ec_ref_device_info.dev_id;
        config.dev_info.ec_ref_dev.sample_rate = req_info->ec_ref_device_info.sample_rate;
        config.dev_info.ec_ref_dev.bits_per_sample = req_info->ec_ref_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "ec_ref - id[%d], sample_rate[%d], bps[%d]", config.dev_info.ec_ref_dev.dev_id, 
                 config.dev_info.ec_ref_dev.sample_rate, config.dev_info.ec_ref_dev.bits_per_sample );
      }  
      
      csd_rc = csdlite_voice_config( &config );
    }
    break;
    
    case QMI_CSD_VOICE_START_REQ_V01: {
      qmi_csd_voice_start_req_msg_v01 *req_info = (qmi_csd_voice_start_req_msg_v01 *)req_c_struct;
      csdlite_voice_config_t start;
      char_t session_id[SESSION_NAME_MAX_LEN_V01 + 1];      
      
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_START_REQ_V01 called" );
      
      strlcpy( session_id, req_info->session_id, sizeof(req_info->session_id) );
      start.session_id = session_id;
      start.cmd_token = req_info->cmd_token;
      start.dev_info.tx_dev.dev_id = 0xFFFFFFFF;
      start.dev_info.rx_dev.dev_id = 0xFFFFFFFF;
      start.dev_info.ec_ref_dev.dev_id = 0xFFFFFFFF;
      
      if( req_info->tx_device_info_valid ) {
        start.dev_info.tx_dev.dev_id = req_info->tx_device_info.dev_id;
        start.dev_info.tx_dev.sample_rate = req_info->tx_device_info.sample_rate;
        start.dev_info.tx_dev.bits_per_sample = req_info->tx_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "tx - id[%d], sample_rate[%d], bps[%d]", start.dev_info.tx_dev.dev_id, 
                 start.dev_info.tx_dev.sample_rate, start.dev_info.tx_dev.bits_per_sample );
      }
      if( req_info->rx_device_info_valid ) {
        start.dev_info.rx_dev.dev_id = req_info->rx_device_info.dev_id;
        start.dev_info.rx_dev.sample_rate = req_info->rx_device_info.sample_rate;
        start.dev_info.rx_dev.bits_per_sample = req_info->rx_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "rx - id[%d], sample_rate[%d], bps[%d]", start.dev_info.rx_dev.dev_id, 
                 start.dev_info.rx_dev.sample_rate, start.dev_info.rx_dev.bits_per_sample );
      }
      if( req_info->ec_ref_device_info_valid ) {
        start.dev_info.ec_ref_dev.dev_id = req_info->ec_ref_device_info.dev_id;
        start.dev_info.ec_ref_dev.sample_rate = req_info->ec_ref_device_info.sample_rate;
        start.dev_info.ec_ref_dev.bits_per_sample = req_info->ec_ref_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "ec_ref - id[%d], sample_rate[%d], bps[%d]", start.dev_info.ec_ref_dev.dev_id, 
                 start.dev_info.ec_ref_dev.sample_rate, start.dev_info.ec_ref_dev.bits_per_sample );
      }  
      
      csd_rc = csdlite_voice_start( &start );
    }
    break;
    
    case QMI_CSD_VOICE_END_REQ_V01: {
      qmi_csd_voice_end_req_msg_v01 *req_info = (qmi_csd_voice_end_req_msg_v01 *)req_c_struct;
      csdlite_voice_end_t end;
              
      end.cmd_token = req_info->cmd_token;
      
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_END_REQ_V01 called" );
      
      csd_rc = csdlite_voice_end( &end );
    }
    break;
    
    case QMI_CSD_VOICE_DEVICE_SWITCH_REQ_V01: {
      qmi_csd_voice_device_switch_req_msg_v01 *req_info = (qmi_csd_voice_device_switch_req_msg_v01 *)req_c_struct;
      csdlite_dev_switch_t dev_switch;
      
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_DEVICE_SWITCH_REQ_V01 called" );
      
      dev_switch.cmd_token = req_info->cmd_token;   
      dev_switch.dev_info.tx_dev.dev_id = req_info->tx_device_info.dev_id;
      dev_switch.dev_info.tx_dev.sample_rate = req_info->tx_device_info.sample_rate;
      dev_switch.dev_info.tx_dev.bits_per_sample = req_info->tx_device_info.bits_per_sample;
      dev_switch.dev_info.rx_dev.dev_id = req_info->rx_device_info.dev_id;
      dev_switch.dev_info.rx_dev.sample_rate = req_info->rx_device_info.sample_rate;
      dev_switch.dev_info.rx_dev.bits_per_sample = req_info->rx_device_info.bits_per_sample;
      dev_switch.dev_info.ec_ref_dev.dev_id = 0xFFFFFFFF;
      
      CSD_MSG( CSD_PRIO_HIGH, "tx - id[%d], sample_rate[%d], bps[%d]", dev_switch.dev_info.tx_dev.dev_id, 
                 dev_switch.dev_info.tx_dev.sample_rate, dev_switch.dev_info.tx_dev.bits_per_sample );
      CSD_MSG( CSD_PRIO_HIGH, "rx - id[%d], sample_rate[%d], bps[%d]", dev_switch.dev_info.rx_dev.dev_id, 
                 dev_switch.dev_info.rx_dev.sample_rate, dev_switch.dev_info.rx_dev.bits_per_sample );
    
      if( req_info->ec_ref_device_info_valid ) {
        dev_switch.dev_info.ec_ref_dev.dev_id = req_info->ec_ref_device_info.dev_id;
        dev_switch.dev_info.ec_ref_dev.sample_rate = req_info->ec_ref_device_info.sample_rate;
        dev_switch.dev_info.ec_ref_dev.bits_per_sample = req_info->ec_ref_device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "ec_ref - id[%d], sample_rate[%d], bps[%d]", dev_switch.dev_info.ec_ref_dev.dev_id, 
                 dev_switch.dev_info.ec_ref_dev.sample_rate, dev_switch.dev_info.ec_ref_dev.bits_per_sample );
      }
    
      csd_rc = csdlite_dev_switch( &dev_switch );
    }
    break;
    
    case QMI_CSD_AFE_LOOPBACK_REQ_V01: {
      qmi_csd_afe_loopback_req_msg_v01 *req_info = (qmi_csd_afe_loopback_req_msg_v01 *)req_c_struct;
      csdlite_afe_loopback_t afe_lb;
      
      afe_lb.cmd_token = req_info->cmd_token;   
      afe_lb.dev_info.tx_dev.dev_id = req_info->tx_device_info.dev_id;
      afe_lb.dev_info.tx_dev.sample_rate = req_info->tx_device_info.sample_rate;
      afe_lb.dev_info.tx_dev.bits_per_sample = req_info->tx_device_info.bits_per_sample;
      afe_lb.dev_info.rx_dev.dev_id = req_info->rx_device_info.dev_id;
      afe_lb.dev_info.rx_dev.sample_rate = req_info->rx_device_info.sample_rate;
      afe_lb.dev_info.rx_dev.bits_per_sample = req_info->rx_device_info.bits_per_sample;
      afe_lb.dev_info.ec_ref_dev.dev_id = 0xFFFFFFFF;
      afe_lb.enable = req_info->enable;
      afe_lb.afe_mode = 1;
    
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_AFE_LOOPBACK_REQ_V01 called"
               " tx - id[%d], sample_rate[%d], bps[%d], rx - id[%d], sample_rate[%d], bps[%d]"
               " enable[%d]",afe_lb.dev_info.tx_dev.dev_id, 
               afe_lb.dev_info.tx_dev.sample_rate, afe_lb.dev_info.tx_dev.bits_per_sample, 
               afe_lb.dev_info.rx_dev.dev_id, afe_lb.dev_info.rx_dev.sample_rate, 
               afe_lb.dev_info.rx_dev.bits_per_sample, afe_lb.enable );  
      
      csd_rc = csdlite_afe_loopback( &afe_lb );
    }
    break;
    
    case QMI_CSD_VOICE_VOLUME_CONTROL_REQ_V01: {
      qmi_csd_voice_volume_control_req_msg_v01 *req_info = (qmi_csd_voice_volume_control_req_msg_v01 *)req_c_struct;
      csdlite_volume_t volume;
      
      volume.cmd_token = req_info->cmd_token;   
      volume.vol_step = req_info->volume_index;
      volume.ramp_duration = req_info->ramp_duration_in_ms;
    
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_VOLUME_CONTROL_REQ_V01 called"
               " volume_index[%d], ramp_duration_in_ms[%d]", volume.vol_step, volume.ramp_duration );
               
      csd_rc = csdlite_volume( &volume );
    }
    break;
    
    case QMI_CSD_VOICE_MUTE_REQ_V01: {
      qmi_csd_voice_mute_req_msg_v01 *req_info = (qmi_csd_voice_mute_req_msg_v01 *)req_c_struct;
      csdlite_mute_t mute;
      
      mute.cmd_token = req_info->cmd_token;   
      mute.mode = (csdlite_mute_mode_t)req_info->mode;
      mute.mute_flag = req_info->enable;
      mute.direction = (csdlite_dev_direction_t)req_info->direction;
      mute.ramp_duration = req_info->ramp_duration_in_ms;
    
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_MUTE_REQ_V01 called"
               " mode[%d], enable[%d], direction[%d], ramp_duration_in_ms[%d]", 
               mute.mode, mute.mute_flag, mute.direction, mute.ramp_duration );
               
      csd_rc = csdlite_mute( &mute );
    }
    break;
    
    case QMI_CSD_DTMF_GENERATION_REQ_V01: {
      qmi_csd_dtmf_generation_req_msg_v01 *req_info = (qmi_csd_dtmf_generation_req_msg_v01 *)req_c_struct;
      csdlite_dtmf_gen_t gen;
      
      memset( &gen, 0, sizeof(gen) );
      
      gen.cmd_token = req_info->cmd_token;   
      gen.direction = (csdlite_dev_direction_t)req_info->direction;
      gen.dtmf_duration_in_ms = req_info->dtmf_duration_in_ms;
      gen.dtmf_high_freq = req_info->high_frequency;
      gen.dtmf_low_freq = req_info->low_frequency;
      gen.dtmf_gain = req_info->gain;
      gen.mix_flag = CSD_DTMF_MIX_WITH_SPEECH_ENABLE_V01; // Set default as DTMF mixes with speech
      gen.dev_info.tx_dev.dev_id = 0xFFFFFFFF;
      gen.dev_info.rx_dev.dev_id = 0xFFFFFFFF;
      gen.dev_info.ec_ref_dev.dev_id = 0xFFFFFFFF;
      gen.cb_func = qmi_csdlite_callback;   
    
      if( req_info->mix_mode_valid )
        gen.mix_flag = req_info->mix_mode;
    
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_DTMF_GENERATION_REQ_V01 called"
               " direction[%d], high_freq[%d], low_frequency[%d], dtmf_duration_in_ms[%d], dtmf_gain[%d]"
               " mix_flag[%d], ", gen.direction, gen.dtmf_high_freq, gen.dtmf_low_freq, 
               gen.dtmf_duration_in_ms, gen.dtmf_gain, gen.mix_flag );
    
      if( req_info->device_info_valid ) {
        csdlite_device_t *tmp;
        
        if( req_info->direction == CSD_DTMF_GENERATION_DIR_TX_V01 ) 
          tmp = &gen.dev_info.tx_dev;
        else if( req_info->direction == CSD_DTMF_GENERATION_DIR_RX_V01 )
          tmp = &gen.dev_info.rx_dev;
        else {
          CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_handle_req: QMI_CSD_DTMF_GENERATION_REQ_V01 called with"
                   " unsupported direction[%d]", req_info->direction );
          csd_rc = CSD_EUNSUPPORTED;
          break;
        }
          
        tmp->dev_id = req_info->device_info.dev_id;                    
        tmp->sample_rate = req_info->device_info.sample_rate;
        tmp->bits_per_sample = req_info->device_info.bits_per_sample;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: dev id[%d], sample_rate[%d], bps[%d]",
                 tmp->dev_id, tmp->sample_rate, tmp->bits_per_sample );        
      }
        
      csd_rc = csdlite_dtmf_generation( &gen );
    }
    break;
    
    case QMI_CSD_VOICE_DTMF_DETECTION_REQ_V01: {
      qmi_csd_voice_dtmf_detection_req_msg_v01 *req_info = (qmi_csd_voice_dtmf_detection_req_msg_v01 *)req_c_struct;
      csdlite_dtmf_detect_t detect;
      
      detect.cmd_token = req_info->cmd_token;   
      detect.direction = (csdlite_dev_direction_t)req_info->direction;
      detect.enable = req_info->enable;
      detect.cb_func = qmi_csdlite_callback;    
    
      CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_VOICE_DTMF_DETECTION_REQ_V01 called"
               " direction[%d], enable[%d]", detect.direction, detect.enable );
               
      csd_rc = csdlite_dtmf_detection( &detect );
    }
    break;
    
    case QMI_CSD_SET_VOICE_FEATURE_REQ_V01: {
      qmi_csd_set_voice_feature_req_msg_v01 *req_info = (qmi_csd_set_voice_feature_req_msg_v01 *)req_c_struct;
      csdlite_feature_t feature;
      uint8_t count = 0;
      
      count = req_info->hdvoice_mode_valid + req_info->slowtalk_mode_valid + req_info->voice_tty_mode_valid;
      if( count != 1 ) {
        CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_handle_req: QMI_CSD_SET_VOICE_FEATURE_REQ_V01 called,"
               " unsupported number of features[%d] to be enabled", count );
        error = QMI_ERR_INVALID_OPERATION_V01;
        break;
      }
      
      feature.cmd_token = req_info->cmd_token;
      
      if( req_info->hdvoice_mode_valid ) {
        feature.feature = CSDLITE_HDVOICE;
        feature.mode = req_info->hdvoice_mode;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_SET_VOICE_FEATURE_REQ_V01 called"
               " hdvoice_mode[%d]", feature.mode );                
      }
      
      if( req_info->slowtalk_mode_valid ) {
        feature.feature = CSDLITE_SLOWTALK;
        feature.mode = req_info->slowtalk_mode;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_SET_VOICE_FEATURE_REQ_V01 called"
               " slowtalk_mode[%d]", feature.mode );                
      }  
      else if( req_info->voice_tty_mode_valid ) {
        feature.feature = CSDLITE_SET_TTY_MODE;
        feature.mode = req_info->voice_tty_mode;
        
        CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: QMI_CSD_SET_VOICE_FEATURE_REQ_V01 called"
               " voice_tty_mode[%d]", feature.mode );                
      }
      
      csd_rc = csdlite_set_voice_feature( &feature );      
    }
    break;
    
    case QMI_CSD_VOICE_RECORD_START_REQ_V01:
    case QMI_CSD_VOICE_RECORD_END_REQ_V01:
    case QMI_CSD_VOICE_PLAYBACK_START_REQ_V01:
    case QMI_CSD_VOICE_PLAYBACK_END_REQ_V01:
    default:
      CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_handle_req: unsupported msg_id[%d]", msg_id );
      csd_rc = CSD_EUNSUPPORTED;
    break;
  }
  
  CSD_MSG( CSD_PRIO_HIGH, "qmi_csdlite_handle_req: msg_id[%d] returns rc[0x%x]", msg_id, csd_rc );
  
  memset( &resp_msg, 0, sizeof(qmi_csd_common_resp_msg_v01) );
  
  resp_msg.cmd_token_valid = TRUE;
  resp_msg.cmd_token = ((qmi_csd_voice_config_req_msg_v01 *)req_c_struct)->cmd_token;  
  
  if( error ) {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = error;
  }
  else if( csd_rc ) {
    resp_msg.qmi_csd_status_code_valid = TRUE;
    resp_msg.qmi_csd_status_code = (qmi_csd_status_v01)csd_rc;    
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_GENERAL_V01;
  }
 
  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp_msg, sizeof(resp_msg) );
  if( resp_err != QMI_CSI_NO_ERR ) {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csdlite_handle_req: qmi_csi_send_resp failed, rc[0x%x]", resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  
  return qmi_rc;
}
