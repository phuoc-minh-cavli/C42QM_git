#ifndef _Q6_MSG_DEF_H_
#define _Q6_MSG_DEF_H_
/*! \file csd_q6msg_def.h
    \brief Q6 DSP Message library
 
    Copyright (C) 2009,2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/utils/q6msglib/inc/csd_q6msg_def.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/07/13   vk      added support for AVSync feature
02/15/13   aim     Added support for LPASS clock control feature.
01/10/13   rp      Added SideTone IIR filter support
08/10/12   rp      Added 9x25 AFE support.
01/20/11   ls      GSM-FR, Flac and Vorbis updates.
04/23/10   zj      initial release

============================================================================*/
#include "mmdefs.h"
#include "aprv2_packet.h"
#include "aprv2_msg_if.h"
#include "aprv2_ids_services.h"
#include "aprv2_ids_domains.h"
#include "adsp_asm_session_commands.h"
#include "adsp_asm_data_commands.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_asm_service_commands.h"
#include "adsp_media_fmt.h"
#include "adsp_adm_api.h"
#include "adsp_afe_service_commands.h"
#include "adsp_core_api.h"


/*! \brief	Domain */
#define ADSP_DOMAIN								APRV2_IDS_DOMAIN_ID_ADSP_V
#define AUDIO_DOMIAN							APRV2_IDS_DOMAIN_ID_APPS_V

/*! \brief local services */
#define CSD_SERVICE_AS							APRV2_IDS_SERVICE_ID_APPS_AS
#define CSD_SERVICE_AC							APRV2_IDS_SERVICE_ID_APPS_AC
#define CSD_SERVICE_DM							APRV2_IDS_SERVICE_ID_APPS_DM
#define CSD_SERVICE_VS							APRV2_IDS_SERVICE_ID_APPS_VS
#define CSD_SERVICE_VC							APRV2_IDS_SERVICE_ID_APPS_VC

#define CSD_AC_ADDR                             ((AUDIO_DOMIAN<<8)|CSD_SERVICE_AC)
#define CSD_AS_ADDR                             ((AUDIO_DOMIAN<<8)|CSD_SERVICE_AS)
#define CSD_DM_ADDR                             ((AUDIO_DOMIAN<<8)|CSD_SERVICE_DM)

/** @brief  Q6 services */
#define ADSP_CORE_SERVICE                       APRV2_IDS_SERVICE_ID_ADSP_CORE_V
#define ADSP_AFE_SERVICE                        APRV2_IDS_SERVICE_ID_ADSP_AFE_V
#define ADSP_VSM_SERVICE                        APRV2_IDS_SERVICE_ID_ADSP_VSM_V
#define ADSP_VPM_SERVICE                        APRV2_IDS_SERVICE_ID_ADSP_VPM_V
#define ADSP_ASM_SERVICE                        APRV2_IDS_SERVICE_ID_ADSP_ASM_V
#define ADSP_ADM_SERVICE                        APRV2_IDS_SERVICE_ID_ADSP_ADM_V

#define ADSP_AFE_ADDR                           ((ADSP_DOMAIN<<8)|ADSP_AFE_SERVICE)
#define ADSP_ADM_ADDR                           ((ADSP_DOMAIN<<8)|ADSP_ADM_SERVICE)
#define ADSP_ASM_ADDR                           ((ADSP_DOMAIN<<8)|ADSP_ASM_SERVICE)

/*! \brief	Q6 message version */
#define Q6MSG_VERSION                           APRV2_PKT_VERSION_V

/*! \brief	Q6 message type */
#define Q6MSG_TYPE_SQCMD                        APRV2_PKT_MSGTYPE_SEQCMD_V
#define Q6MSG_TYPE_NSCMD                        APRV2_PKT_MSGTYPE_NSEQCMD_V
#define Q6MSG_TYPE_RSP                          APRV2_PKT_MSGTYPE_CMDRSP_V
#define Q6MSG_TYPE_EVT                          APRV2_PKT_MSGTYPE_EVENT_V

/*! \def Default AFE PORT GAIN setting */
#define AFE_DEFAULT_GAIN                        0x2000

/** @brief  q6 decoder format block */
struct q6msg_stream_w_fmt
{
    asm_data_cmd_media_fmt_update_v2_t    hdr;
    union
    {
        asm_multi_channel_pcm_fmt_blk_v2_t  multi_ch_pcm;
        asm_adpcm_fmt_blk_t                 adpcm;
        asm_aac_fmt_blk_v2_t                aac;
        asm_midi_fmt_blk_t                  midi;
        asm_wmastdv9_fmt_blk_v2_t           wma_v9;
        asm_wmaprov10_fmt_blk_v2_t          wma_v10;
        asm_amrwbplus_fmt_blk_v2_t          amrwbplus;
        asm_g711_alaw_fmt_blk_t             g711_alaw;
        asm_g711_mlaw_fmt_blk_t             g711_ulaw;
        asm_vorbis_fmt_blk_t                vorbis;
        asm_flac_fmt_blk_t                  flac;
    }u;
};

/** @brief  q6 encoder format block */
struct q6msg_stream_fmt_r
{
    asm_enc_cfg_blk_param_v2_t             hdr;
    union
    {
        asm_multi_channel_pcm_enc_cfg_v2_t  multi_ch_pcm;
        asm_adpcm_enc_cfg_t                 adpcm;
        asm_aac_enc_cfg_v2_t                aac;
        asm_amrnb_enc_cfg_t                 amrnb;
        asm_amrwb_enc_cfg_t                 amrwb;
        asm_v13k_enc_cfg_t                  qcepl13k;
        asm_evrc_enc_cfg_t                  evrc;
        asm_evrcb_enc_cfg_t                 evrcb;
        asm_evrcwb_enc_cfg_t                evrcwb;
        asm_sbc_enc_cfg_t                   sbc;
        asm_g711_alaw_enc_cfg_t             g711_alaw;
        asm_g711_mlaw_enc_cfg_t             g711_ulaw;

	}u;
};

/** @brief  q6 encoder */
struct q6msg_stream_r_fmt
{
	asm_stream_cmd_set_encdec_param_t		hdr;
	struct q6msg_stream_fmt_r				payload;
};


/** \brief EQ payload */
#define Q6MSG_EQ_BAND_MAX					12
struct q6msg_eq_data
{
	asm_eq_params_t				hdr;
	asm_eq_per_band_params_t	band[Q6MSG_EQ_BAND_MAX];
};

#define ASM_STREAM_PP_MAX_SIZE						1
struct q6msg_pp_payload
{
    asm_stream_param_data_v2_t                hdr;
    union {
        asm_volume_ctrl_master_gain_t       master_gain;
        asm_volume_ctrl_lr_chan_gain_t      stereo_gain;
        asm_volume_ctrl_mute_config_t       mute;
        asm_soft_step_volume_params_t       soft_vol_stepping;
        asm_iir_tuning_filter_enable_t      iir_cmd;
        asm_iir_tuning_filter_pregain_t     iir_pregain;
        asm_iir_filter_config_params_t      iir_config;
        asm_mbdrc_enable_t                  mbdrc_cmd;
        asm_mbdrc_config_params_t           mbdrc_config;
        asm_subband_drc_config_params_t     sb_drc_config;
        struct q6msg_eq_data                eq;
        asm_qcpr_enable_t                   qcpr_enable;
        asm_qcpr_preset_config_t            qcpr_preset;
        asm_qcpr_strength_t                 qcpr_strength;
        asm_qcpr_output_t                   qcpr_output;
        asm_qcpr_geo_t                      qcpr_geo;
        asm_qcpr_delay_t                    qcpr_delay;
        asm_pbe_enable_t                    pbe_cmd;
        asm_pbe_params_t                    pbe_config;
        asm_spa_enable_t                    spa_cmd;
        asm_spa_param_t                     spa_config;
        asm_aac_sbr_ps_flag_param_t         aac_sbr_ps;
        asm_soft_pause_params_t             soft_pause_resume_cfg;
    }u;
};

/** @brief  q6 msg structure for ASM_STREAM_CMD_SET_PP_PARAMS_V2
 *  @note   we don't use inband acdb push down for now
 *  */
struct q6msg_strm_set_pp
{
    asm_stream_cmd_set_pp_params_v2_t            hdr;
    struct q6msg_pp_payload    data[ASM_STREAM_PP_MAX_SIZE];
};

struct q6msg_strm_set_encdec_payload {
    union {
        asm_aac_sbr_ps_flag_param_t       aac_sbr_flag;
        asm_aac_dual_mono_mapping_param_t dual_mono;
        asm_bitrate_param_t               bit_rate;
//        asm_immediate_dec_param_t         immediate_dec_mode;        //Deprecated API
//      asm_wmapro_generic_param_t          wmapro_generic_param;        //Deprecated API
//      asm_ac3_channel_config_param_t      eac3_channel_cfg;            //Deprecated API
//      asm_ac3_channel_routing_param_t      eac3_cr_cfg;
        asm_ac3_generic_param_t              eac3_generic_param;
        asm_dec_out_chan_map_param_t      dec_out_channel_cfg;
    } u;
};
struct q6msg_strm_set_encdec_params
{
    asm_stream_cmd_set_encdec_param_t       hdr;
    struct q6msg_strm_set_encdec_payload    payload;
};

#define CSD_Q6MSG_MEM_REGION_SIZE_MAX        4
struct q6msg_strm_mem_map
{
    asm_cmd_shared_mem_map_regions_t hdr;
    struct avs_shared_map_region_payload_t map[CSD_Q6MSG_MEM_REGION_SIZE_MAX];
};


struct q6msg_set_param
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  union
  {
    afe_param_id_i2s_cfg_t                     i2s_cfg;
    afe_param_id_pcm_cfg_t                     pcm_cfg;
    afe_param_id_digi_mic_cfg_t                digi_mic_cfg;
    afe_param_id_hdmi_multi_chan_audio_cfg_t   hdmi_cfg;
    afe_param_id_internal_bt_fm_cfg_t          bt_fm_cfg;
    afe_param_id_slimbus_cfg_t                 slimbus_cfg;
    afe_param_id_rt_proxy_port_cfg_t           rt_proxy_cfg;
    afe_param_id_pseudo_port_cfg_t             pseudo_cfg;
  }u;	
  afe_port_param_data_v2_t         param_data_delay;
  afe_param_id_device_hw_delay_t   hw_delay;
};

struct q6msg_set_loop_back_gain
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  afe_loopback_gain_per_path_param_t  gain;
};

struct q6msg_set_loopback
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  afe_loopback_cfg_v1_t           loopback_cfg;
};

struct q6msg_set_lpaif_clk
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  afe_param_id_lpaif_clk_cfg_t    lpaif_clk_cfg;
};

struct q6msg_param_enable
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  afe_mod_enable_param_t          param_enable;
};

struct q6msg_set_ff_spkr_prot_cfg
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  afe_spkr_prot_cfg_param_v1_t    ff_spkr_prot_cfg;
};

struct q6msg_set_ff_spkr_prot_enable
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  afe_mod_enable_param_t          ff_spkr_prot_enable;
};

struct q6msg_lpass_core_shared_clk_cfg
{
  afe_port_cmd_set_param_v2_t                 set_param;
  afe_port_param_data_v2_t                    param_data;
  afe_param_id_lpass_core_shared_clk_cfg_t    lpass_core_shared_clk_cfg;
};

#define CSD_Q6MSG_IIR_FILTER_STAGES_MAX   10
#define CSD_Q6MSG_IIR_FILTER_COEFF_PER_STAGE   5

typedef struct 
{
   uint16_t  num_bi_quad_stages;
   uint16_t  pre_gain;
/* max 10 filter stage coefficients
   each stage has 5 coeffieients of size int32_t
   10 * 5 * 4    =  200 bytes
   max 10 num shift factors each of size int16_t 
   10 * 2  =   20 bytes
   total 220 bytes or  55 integers */
   int32_t   filter_coeff_shift[55]; 
}q6msg_config_iir_filter;

struct q6msg_config_sidetone_iir_filter
{
  afe_port_cmd_set_param_v2_t     set_param;
  afe_port_param_data_v2_t        param_data;
  q6msg_config_iir_filter         iir_filter;
};


/** @brief  q6 msg structure for AFE get Params command */
struct q6msg_afe_get_params_rsp
{
  afe_port_param_data_v2_t	param_data;
  union
  {
    afe_fbsp_calib_res_cfg_v1_t	fbsp_calib_res_cfg;
  }u;
};

/** @brief  q6 msg structure for ASM_STREAM_CMD_GET_PP_PARAMS
*  @note
*  */
struct q6msg_aprv2_rsp_result
{
  uint32_t status;
  union
  {
    struct q6msg_afe_get_params_rsp	afe_rsp;
  }u;
};

/**********************************************************************/
/** @brief  q6 top level msg structure                                */
/**********************************************************************/
struct q6msg
{
    struct aprv2_packet_t hdr;
    union {
        /**------------------------------------------------------*/
        /*! \brief  asm messages */
        /**------------------------------------------------------*/
        asm_stream_cmd_open_write_v2_t                  open_w;
        asm_stream_cmd_open_write_v3_t                  open_w_v2;
        asm_stream_cmd_open_read_v2_t                   open_r;
        asm_stream_cmd_open_read_v3_t                   open_r_v2;
        asm_stream_cmd_open_readwrite_v2_t              open_rw;
        asm_stream_cmd_open_loopback_v2_t               open_lb;
        struct q6msg_stream_w_fmt                       set_fmt_w;
        struct q6msg_stream_r_fmt                       set_fmt_r;
        struct q6msg_strm_set_pp                        set_pp;

        asm_session_cmd_run_v2_t                        sess_run;
        asm_session_cmd_adjust_session_clock_v2_t       sess_adjust_time;
        asm_session_cmdrsp_adjust_session_clock_v2_t    sess_adjust_time_rsp;
        asm_session_cmd_rgstr_rx_underflow_t            sess_rx_reg_evt;
        asm_session_cmd_reg_tx_overflow_t               sess_tx_reg_evt;

        asm_data_cmd_write_v2_t                         data_w;
        asm_data_event_write_done_v2_t                  data_w_done;
        asm_data_cmd_read_v2_t                          data_r;
        asm_data_event_read_done_v2_t                   data_r_done;
        asm_data_cmd_spa_v2_t                           spa_data_r;
        asm_data_event_spa_done_v2_t                    spa_data_r_done;
        asm_data_event_sr_cm_change_notify_t            sr_cm_notification;
        asm_data_event_enc_sr_cm_change_notify_t        enc_sr_cm_notification;
//        asm_svc_cmdrsp_get_wallclk_time_t             get_wall_clk;        //Deprecated API
        asm_session_cmdrsp_get_session_time_v3_t        get_session_time;
        struct q6msg_strm_mem_map                       asm_map;
		asm_cmdrsp_shared_mem_map_regions_t				asm_map_rsp;
        asm_cmd_shared_mem_unmap_regions_t              asm_unmap;
        struct q6msg_strm_set_encdec_params             asm_set_encdec_params;
        asm_stream_cmd_dtmf_ctl_t                       asm_dtmf_ctl;

        /**------------------------------------------------------*/
        /*! \brief  adm messages */
        /**------------------------------------------------------*/
        adm_cmd_matrix_map_routings_v5_t                adm_map;
        adm_cmd_device_open_v5_t                        adm_copp_multi_chanel_open;
        adm_cmd_set_pp_params_v5_t                      adm_copp_set_pp;
        adm_cmd_rsp_device_open_v5_t                    adm_copp_multi_channel_open_rsp;
        adm_cmd_matrix_ramp_gains_v5_t                  adm_matrix_ramp_gain;
        /**------------------------------------------------------*/
        /*! \brief  afe messages */
        /**------------------------------------------------------*/
        /** DTMF commands: Unchanged */
        afe_ports_cmd_dtmf_ctl_t                    afe_dmtf_ctl;
        /** AFE_PORT_CMD_DEVICE_START */
        afe_port_cmd_device_start_t                 afe_port_start;
        /** AFE_PORT_CMD_DEVICE_STOP */
        afe_port_cmd_device_stop_t                  afe_port_stop;
        afe_port_cmd_devices_restart_t              afe_port_restart;
        struct q6msg_set_loop_back_gain             afe_set_loopback_gain_path;
        struct q6msg_set_loopback                   afe_loopback;
        struct q6msg_set_ff_spkr_prot_cfg           afe_ff_spkr_prot_cfg;
        struct q6msg_set_ff_spkr_prot_enable        afe_ff_spkr_prot_enable;
        struct q6msg_set_param                      afe_set_param_v2;
        struct q6msg_param_enable                   param_enable;
        struct q6msg_config_sidetone_iir_filter     sidetone_iir_filter;
        struct q6msg_set_lpaif_clk                  lpaif_clk;
        afe_port_cmd_get_param_v2_t                 afe_get_param_v2;
        struct q6msg_lpass_core_shared_clk_cfg      afe_lpass_core_shared_clk_cfg;
        /**------------------------------------------------------*/
        /*! \brief core messages */
        /**------------------------------------------------------*/
        avcs_service_cmd_rsp_get_api_versions_t     acs_get_api_versions;        
        /** SIDETONE AND LOOPBACK: To be replaced with  AFE_PARAM_ID_LOOPBACK_CONFIG
        No change from 1.2 
        // GAIN And OTHER Unused commands
        afe_port_cmd_apply_gain_t                   afe_gain;
        afe_port_digi_mic_vol_cfg_t                 afe_dig_mic_vol_cfg;
        afe_test_codec_loopback_ctrl_t              afe_codec_loopback;
        afe_hw_mixer_ctl_t                          afe_mixer_ctl;
        afe_test_dma_loopback_ctrl_t                afe_dma_loopback;
        */
        
        /**------------------------------------------------------*/
        /*! \brief  APR messages */
        /**------------------------------------------------------*/
        aprv2_ibasic_rsp_result_t                   rsp;
        struct q6msg_aprv2_rsp_result               rsp_w_data;
    }u;
};

#endif /* _Q6_MSG_DEF_H_ */

