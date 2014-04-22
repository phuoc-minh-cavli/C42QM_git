#ifndef _Q6_MSG_H_
#define _Q6_MSG_H_
/*! \file q6msg.h
    \brief Q6 DSP Message library interface functions
    
    Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated. All rights reserved. Licensed Material - Restricted Use.
    Qualcomm Confidential and Proprietary.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/utils/q6msglib/inc/csd_q6msg.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/10   zj      initial release

============================================================================*/
#include "mmdefs.h"
#include "csd_q6msg_def.h"
#include "csd_as_fmt.h"
#include "csd_as.h"
#include "csd_asac_intf.h"



#define Q6MSG_SUCCESS           0
#define Q6MSG_FAILURE           -1
#define Q6MSG_INVALID_PARAM     -2
#define Q6MSG_INVALID_FMT_SIZE  -3
#define Q6MSG_INVALID_MSG       -4
#define Q6MSG_UNSUPPORTED       -5


/** @brief  constant for the packed Q6 message length in byte */
#define Q6MSG_PACKED_HEADER_SIZE       20
#define Q6MSG_PACKED_BUF_MAX_SIZE      (sizeof(struct q6msg))


/** @brief  Internal MACRO for set and get fields from APR
 *          header.
 *  For get operation, x is the uint32_t that contains the filed
 *  to be get.
 *  For set operation, x is the uint32_t that we want to set, a
 *  is the uint32_t that contains the value for the field we
 *  want to set
 *  */
#define Q6MSG_GET_HDR_VER(x)     (x&0xF)
#define Q6MSG_GET_HDR_SIZE(x)    ((x&0xF0)>>2)
#define Q6MSG_GET_MSG_TYPE(x)    ((x&0x300)>>8)
#define Q6MSG_GET_PACKET_SIZE(x) ((x&0xFFFF0000)>>16)

#define Q6MSG_SET_HDR_VER(x, a)     {x &=~0xF; x|= (a&0xF);}
#define Q6MSG_SET_HDR_SIZE(x, a)    {x&=~0xF0; x|=(((a>>2)&0xF)<<4);}
#define Q6MSG_SET_MSG_TYPE(x, a)    {x&=~0x300; x|=((a&0x3)<<8);}
#define Q6MSG_SET_PACKET_SIZE(x, a) {x&=~0xFFFF0000; x|=((a&0xFFFF)<<16);}

#define Q6MSG_GET_SESSION(x)        ((x&0xFF00)>>8)
#define Q6MSG_GET_STREAM(x)         (x&0xFF)

#define Q6MSG_SET_SESSION(x, a)     {x&=~0xFF00;x|= ((a&0xFF)<<8);}
#define Q6MSG_SET_STREAM(x, a)      {x&=~0xFF;x|= a&0xFF;}

/** @brief Get payload size in bytes. X is the uint32_t field
 *         continas the packet size in byte and header size in
 *         uint32_t
 *  */
#define Q6MSG_GET_PAYLOAD_SIZE(x)   (((x&0xFFFF0000)>>16)-((x&0xF0)>>2))

/** @brief  This funciton generate a client transaction number
 *  @return ositive number
 *   */
uint32_t csd_q6msg_gen_apr_token(void);


int32_t csd_q6msg_create_stream_open(struct q6msg *msg, struct csd_as_open_v2 *csd_open);
int32_t csd_q6msg_create_stream_open_v2(struct q6msg *msg, struct csd_as_open_v2 *csd_open);
int32_t csd_q6msg_create_session_run(struct q6msg *msg, struct csd_as_ts *param);
int32_t csd_q6msg_create_stream_w_data(struct q6msg *msg, struct csd_as_open_v2 *csd_open,
                                     struct csd_as_buffer *buf,uint32_t mem_map_handle);
int32_t csd_q6msg_create_stream_r_data(struct q6msg *msg, struct csd_as_open_v2 *csd_open,
                                     struct csd_as_buffer *buf,uint32_t mem_map_handle);
int32_t csd_q6msg_create_matrix_map(struct q6msg* msg, struct csd_ac_as_dev_enable_disable* param);
int32_t csd_q6msg_get_stream_map_size(uint32_t* size,
									struct csd_ac_as_dev_enable_disable * param);
int32_t csd_q6msg_create_stream_fmt_rx(struct q6msg *msg, 
									struct csd_as_fmt_cfg * fmt);
int32_t csd_q6msg_create_stream_fmt_tx(struct q6msg *msg, 
									struct csd_as_fmt_cfg * fmt,
									uint32_t frames_per_buf);
int32_t csd_q6msg_create_eq(struct q6msg *msg, uint32_t len, struct csd_aud_pp_eq_enable* enable);
int32_t csd_q6msg_create_mute(struct q6msg *msg,struct csd_aud_pp_vol_mute* mute);
uint32_t csd_q6msg_get_adm_matrix_ramp_gain_size(uint32_t* as_handles, uint32_t num_as, 
											   uint32_t num_ac);
int32_t csd_q6msg_create_adm_matrix_ramp_gain(struct q6msg *msg, 
											uint32_t* as_handles, uint32_t num_as,   
											uint32_t* ac_array, uint32_t num_ac);
int32_t csd_q6msg_create_adm_matrix_ramp_gain_digital(struct q6msg *msg, uint32_t copp_non_smooth_step,
												uint32_t* as_handles, uint32_t num_as,
												uint32_t* ac_array, uint32_t num_ac);
int32_t csd_q6msg_create_stereo_gain(struct q6msg *msg, asm_volume_ctrl_lr_chan_gain_t* gain);
int32_t csd_q6msg_create_stream_soft_vol_stepping(uint32_t app_type,struct q6msg *msg);
int32_t csd_q6msg_create_stream_mem_map(struct q6msg *msg,uint16_t count, uint8_t* buf, uint32_t size, uint16_t mem_type);
int32_t csd_q6msg_create_stream_mem_unmap(struct q6msg *msg, uint16_t count, uint8_t* buf);
int32_t csd_q6msg_create_spa_enable(struct q6msg *msg,struct csd_aud_pp_spa_config* enable, uint32_t len);
int32_t csd_q6msg_create_spa_config(struct q6msg *msg,struct csd_aud_pp_spa_config* config);
int32_t csd_q6msg_create_spa_disable(struct q6msg *msg);
int32_t csd_q6msg_create_stream_spa_data(struct q6msg *msg, struct csd_as_spa_buffer *buf);
int32_t csd_q6msg_create_tsm_enable(struct q6msg *msg,struct csd_aud_pp_tsm_enable* enable);
int32_t csd_q6msg_create_tsm_config(struct q6msg *msg,struct csd_aud_pp_tsm_config* config);
int32_t csd_q6msg_create_qcpr_enable(struct q6msg *msg, uint32_t len, struct csd_aud_pp_qcpr_config* enable);
int32_t csd_q6msg_create_qcpr_config(struct q6msg *msg, uint32_t len, struct csd_aud_pp_qcpr_config* config);
int32_t csd_q6msg_create_qcpr_disable(struct q6msg *msg, uint32_t len);
int32_t csd_q6msg_create_aac_sbr_ps(struct q6msg *msg, struct csd_as_set_aac_flag* param);
int32_t csd_q6msg_adjust_session_clock(struct q6msg *msg, struct csd_as_adjust_session_clock* data);
int32_t csd_q6msg_create_dual_mono_remap(struct q6msg *msg, struct csd_as_dual_mono_remap* data);
int32_t csd_q6msg_create_as_soft_pause_cfg(struct q6msg *msg,uint32_t* msg_len);
int32_t csd_q6msg_create_dtmf_ctl(struct q6msg *msg,
                                  struct csd_as_dtmf_ctl* dtmf_ctl,
                                  uint32_t* ac_array, 
                                  uint32_t num_ac);
int32_t csd_q6msg_create_enc_br_update(struct q6msg *msg, struct csd_as_stream_enc_bit_rate_update* data);
int32_t csd_q6msg_create_decoder_multi_channel_cfg(struct q6msg *msg, struct csd_as_stream_config_multi_channel* channel_cfg);
//int32_t csd_q6msg_create_decoder_ac3_multi_channel_cfg(struct q6msg *msg, struct csd_as_stream_config_multi_channel* channel_cfg);
int32_t csd_q6msg_create_matrix_map_per_context(struct q6msg* msg, struct csd_ac_as_dev_enable_disable* param);
int32_t csd_q6msg_get_stream_map_size_per_context(uint32_t *size, struct csd_ac_as_dev_enable_disable * param);
int32_t csd_q6msg_create_ac3_feature_param_karaoke(struct q6msg *msg, struct csd_as_ac3_karaoke *cfg);
int32_t csd_q6msg_create_ac3_feature_param_drc(struct q6msg *msg,struct csd_as_ac3_drc *cfg);
int32_t csd_q6msg_create_ac3_feature_param_drc_boost(struct q6msg *msg,struct csd_as_ac3_boost_scalefactor *cfg);
int32_t csd_q6msg_create_ac3_feature_param_drc_cut(struct q6msg *msg,struct csd_as_ac3_cut_scalefactor *cfg);
int32_t csd_q6msg_create_ac3_feature_param_dual_mono(struct q6msg *msg, struct csd_as_ac3_dual_mono *cfg);
int32_t csd_q6msg_create_ac3_feature_param_stereo_downmix(struct q6msg *msg, struct csd_as_ac3_stereo_downmix *cfg);
int32_t csd_q6msg_get_digital_master_gain_from_non_smooth_step(void *buf, uint32_t buf_len, uint32_t vol_index,
		                                                       uint32_t app_type, uint32_t dev_id, uint16_t *master_gain);
int32_t csd_q6msg_get_digital_stereo_gain_from_non_smooth_step(void *buf, uint32_t buf_len, uint32_t vol_index,
		                                                       uint32_t app_type, uint32_t dev_id,
		                                                       asm_volume_ctrl_lr_chan_gain_t* stereo_gain);
int32_t csd_q6msg_get_soft_stepping_from_non_smooth_step(void *buf, uint32_t buf_len, uint32_t vol_index,
		                                                 uint32_t app_type, uint32_t dev_id,
		                                                 asm_soft_step_volume_params_t* soft_stepping);
#endif /* _Q6_MSG_H_ */

