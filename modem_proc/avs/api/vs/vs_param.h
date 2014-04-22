#ifndef __VS_PARAM_H__
#define __VS_PARAM_H__

/**
  @file  vs_voc.h
  @brief This file contains vocoder interface definitions of the Vocoder
         Services (VS).
*/

/*
  ============================================================================
  Copyright (C) 2017, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_param.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vs_common.h"


/****************************************************************************
 *  VS VOC PARAM INTERFACE DEFINITIONS
 ****************************************************************************/

/** 
 * MEDIA-ID Parameter definition.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure media-id.
 * Once media-id is configured, client can set media-id specific configuration
 * i.e. DTX, ENC_RATE, etc.
 *
 * Dependencies: Vocoder services must be in DISABLED state.
 */
#define VS_PARAM_VOC_MEDIA_ID ( 0x000132C5 )

typedef struct vs_param_voc_media_id_t {

  uint32_t media_id;
    /**< Specifies the Rx vocoder type.
      *
      *  @values, refer to VS_VOCXXX_MEDIA_ID definitions.
      */
} vs_param_voc_media_id_t;


/** 
 * Timing parameter definitions.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure timing 
 * offsets for vocoder packet exchange.
 *
 * Dependencies: Vocoder services must be in DISABLED state.
 */
#define VS_PARAM_VOC_TIMING_OFFSETS ( 0x000132C6 )

typedef struct vs_param_voc_timing_offsets_t {

  uint32_t enc_offset;
    /**< Offset in microseconds from the VFR to deliver a Tx vocoder packet.
      *
      *  @values < 20000 microseconds.
      */
  uint32_t dec_req_offset;
    /**< Offset in microseconds from the VFR to request for an Rx vocoder packet.
      *
      *  @values < 20000 microseconds.
      */
  uint32_t dec_offset;
    /**< Offset in microseconds from the VFR to indicate the deadline to
      *  receive an Rx vocoder packet.
      *
      *  @values < 20000 microseconds.
      */
  uint32_t vfr_mode;
    /**< Vocoder frame synchronization mode.
      *
      *  @values
      *   - 0: SOFT_VFR - VFR sourced by 20ms software interupt.
      *   - 1: HARD_VFR - VFR sourced by 20ms hardware interupt.
      */
} vs_param_voc_timing_offsets_t;


/** 
 * Voice Timing Reference parameter definitions.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure absolute 
 * read/write timing references for vocoder packet exchange.
 *
 * Dependencies: Vocoder services must be in DISABLED state.
 */
#define VS_PARAM_VOC_TIMING_REFERENCES ( 0x000132C7 )

typedef struct vs_param_voc_timing_reference_t {

  uint64_t read_ref_timestamp_us;
    /**< Client's encoder packet exchange time reference (in microseconds).
      *
      *  The time reference is used to align the encoder's timeline to the
      *  client's timeline.
      *
      *  The clock source to be used for interpreting the timestamp is
      *  product-specific. Currently, the QTimer is used as the timing reference
      *  for all products.
      */
  uint64_t write_ref_timestamp_us;
    /**< Client's decoder packet exchange time reference (in microseconds).
      *
      *  The time reference is used to align the encoder's timeline to the
      *  client's timeline.
      *
      *  The clock source to be used for interpreting the timestamp is
      *  product-specific. Currently, the QTimer is used as the timing reference
      *  for all products.
      */
  uint32_t vfr_mode;
    /**< Vocoder frame synchronization mode.
      *
      *  @values
      *   - 0: SOFT_VFR - VFR sourced by vfr_cycle_duration_ms software interupt.
      *   - 1: HARD_VFR - VFR sourced by 20ms hardware interupt.
      */
  uint32_t vfr_cycle_duration_ms;
    /**< Vocoder frame reference duration in milliseconds.
      *  If vfr_mode is #HARD_VFR, 20ms duration is selected.
      *  If pktexg mode is #OUTOFBAND, 20ms duration is selected.
      *
      *  @values N*20, where N is an integer.
      */
} vs_param_voc_timing_reference_t;


/** 
 * Vocoder packet exchange event enable parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to enable/disable vocoder 
 * packet exchange event during voice call.
 *
 * Dependency: vocoder should be in DISABLED state.
 */
#define VS_PARAM_VOC_PKTEXG_EVENT_ENABLE     ( 0x000132C8 )

typedef struct vs_param_voc_pktexg_event_enable_t {

  uint8_t enable_flag;
    /**< VOC PKTEXG event enable/disable flag.
      *
      *  @values:
      *    - 0: DISABLE (this is default mode).
      *    - 1: ENABLE.
      */
} vs_param_voc_pktexg_event_enable_t;


/** 
 * TTY char exchange enable parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to enable/disable OOBTTY 
 * char exchange during TTY call.
 *
 * Dependency: vocoder should be in DISABLED state.
 */
#define VS_PARAM_OOBTTY_ENABLE     ( 0x000132C9 )

typedef struct vs_param_oobtty_enable_t {

  uint8_t enable_flag;
    /**< OOBTTY enable/disable flag.
      *
      *  @values:
      *    - 0: DISABLED.
      *    - 1: ENABLED
      */
} vs_param_oobtty_enable_t;

/** 
 * MEDIA-ID Parameter definition.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure sample rates 
 * for voice chain post/pre processing, if not set max sample rate supported
 * by vocoder will be set. It is only applicable to EVS vocoder.
 *
 * Dependencies: Vocoder services must be in DISABLED state.
 */
#define VS_PARAM_MAX_PP_SAMPLE_RATE ( 0x000132CA )

typedef struct vs_param_max_pp_sample_rate_t {

  uint32_t rx_pp_sample_rate;
    /**< Rx sampling rate in Khz supported by media_id.
      *
      *  @values:
      *    - 8000  (NB)
      *    - 16000 (WB)
      *    - 32000 (SWB)
      *    - 48000 (FB)
      */
  uint32_t tx_pp_sample_rate;
    /**< Tx sampling rate in Khz supported by media_id.
      *
      *  @values:
      *    - 8000  (NB)
      *    - 16000 (WB)
      *    - 32000 (SWB)
      *    - 48000 (FB)
      */
} vs_param_max_pp_sample_rate_t;


/** 
 * DTX parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to enable or 
 * disable DTX on encoder processing. It can be modified during 
 * vocoder steady state. 
 */
#define VS_PARAM_ENC_DTX ( 0x000132CB )

typedef struct vs_param_enc_dtx_t {

  uint8_t dtx;
    /**< DTX Mode.
      *
      *  @values:
      *  - 0x00 (DTX DISABLE)
      *  - 0x01 (DTX ENABLE)
      */
} vs_param_enc_dtx_t;


/** 
 * Average encoder rate parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure encoder rate
 *  during voice call. It can be modified during vocoder steady state.
 *
 * Please refer to vocoder specific interface definition i.e. vs_vocxxx.h for 
 * applicability.
 */
#define VS_PARAM_ENC_RATE ( 0x000132CC )

typedef struct vs_param_enc_rate_t {

  uint8_t rate;
    /**< Average encoder bit rate supported by vocoder.
      *
      * Please refer to vocoder specifc enumerations for encoder rate aka
      * codec mode.
      */
} vs_param_enc_rate_t;

/** 
 * Encoder bandwidth parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to set encoder operating
 * mode/bandwidth, applicable only to EVS vocoder.
 */
#define VS_PARAM_ENC_OPERATING_MODE ( 0x000132CD )

typedef struct vs_param_enc_operating_mode_t {

  uint8_t rate;
    /**< encoder bit rate supported by vocoder.
      *
      * Please refer to EVS specifc enumerations for encoder rate aka
      * codec mode.
      */
  vs_voc_bandwidth_t bandwidth;
    /**< Operating audio bandwidth for the encoder. */
} vs_param_enc_operating_mode_t;



/** 
 * Channel awareness mode parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to set encoder 
 * channel awareness, it is only applicable to EVS vocoder.
 */
#define VS_PARAM_ENC_CHANNEL_AWARE_MODE ( 0x000132CE )

typedef struct vs_param_enc_channel_aware_mode_t {

  uint8_t channel_awareness_mode;
    /**< Channel Awareness Mode.
      *
      *  @values:
      *  - 0x00 (CA DISABLE)
      *  - 0x01 (CA ENABLE)
      */
  uint8_t fer_rate;
    /**< Forward Error rate threshold.
      *
      *  @values:
      *  - 0x00 (FER RATE LOW)
      *  - 0x01 (FER RATE HIGH)
      */
  uint8_t fec_offset;
    /**< Specifies the forward-error correction offset:
      *
      *  @values:
      *   - 0x02 (FEC_OFFSET_2)
      *   - 0x03 (FEC_OFFSET_3)
      *   - 0x05 (FEC_OFFSET_5)
      *   - 0x07 (FEC_OFFSET_7)
      */
} vs_param_enc_channel_aware_mode_t;


/** 
 * MINMAX frame rate parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to set minimum and maximum
 * frame rates for encoder, applicable only to CDMA vocoders.
 */
#define VS_PARAM_ENC_MINMAX_FRAME_RATE ( 0x000132CF )

typedef struct vs_param_enc_minmax_rate_t {

  uint8_t min_rate;
    /**< Specifies the lower boundary of the encoder rate. */
  uint8_t max_rate;
    /**< Specifies the upper boundary of the encoder rate.. */
} vs_param_enc_minmax_rate_t;


/** 
 * Packet loss concealment parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to enable/disable PLC,
 * EVS and G711 vocoders support this parameter. It can be modified 
 * during vocoder steady state.
 */
#define VS_PARAM_VOC_PLC ( 0x00010E9B )

typedef struct vs_param_voc_plc_t {

  uint8_t plc;
    /**< PLC Mode.
      *
      *  @values:
      *  - 0x00 (PLC DISABLE)
      *  - 0x01 (PLC ENABLE)
      */
} vs_param_voc_plc_t;


/** 
 * Tap-point config parameter definition for vocoder HPCM.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure 
 * a tap-point or to make any change to existing configuration.
 *
 * Dependencies: Vocoder hpcm must be in DISABLED state.
 */
#define VS_PARAM_HPCM_TAP_POINT_CONFIG ( 0x000132D0 )

typedef struct vs_param_hpcm_tap_point_config_t {

  uint32_t tap_point_id;
    /**< Identifies the HPCM tap point.
      *
      * @values:
      *   - VS_HPCM_TAP_POINT_STREAM_TX
      *   - VS_HPCM_TAP_POINT_STREAM_RX
      */
  vs_hpcm_mode_t mode;
    /**< Mode of operation for tap-point.
      *
      * Please refer to vs_hpcm_mode_t.
      */
  uint32_t sample_rate;
    /**< Sampling rate of the tap point. If the tap point is read/write, both
      *  the read/write sampling rates are the same.
      *
      *  @values
      *    - 8000  (NB),
      *    - 16000 (WB).
      *    - 32000 (SWB),
      *    - 48000 (FB).
      */
} vs_param_hpcm_tap_point_config_t;

/** 
 * Fixed/Adaptive DTX parameter.
 * 
 * The client uses VS_COMMON_CMD_SET_PARAM command to configure 
 * EVS encoder in either fixed or adaptive DTX interval.
 */
#define VS_PARAM_ENC_DTX_V2 ( 0x00013343 )

typedef struct vs_param_enc_dtx_v2_t {

  uint32_t version;
    /**< Version value
      *
      *  @values:
      *  - 0x01 
           Supports adaptive DTX
           Maximum and minimum number of SID frames are not configurable.
      */

  uint32_t dtx_mode;
    /**< DTX Mode.
      *
      *  @values:
      *  - 0x00 (DTX DISABLE)
            
      *  - 0x01 (DTX ENABLE)
            Number of SID frames fixed to 8

      *  - 0x02 (Adaptive DTX ENABLE- applicable only for EVS vocoders)
             Minimum number of SID frames : 8
             Maximum number of SID frames : 50
      */
} vs_param_enc_dtx_v2_t;


#endif /* __VS_PARAM_H__ */

