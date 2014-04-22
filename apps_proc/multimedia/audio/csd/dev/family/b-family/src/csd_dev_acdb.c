/*==============================================================================
  FILE:         csd_dev_acdb.c

  OVERVIEW:     The objective of this module is to provide an interface for 
                device modules to query ACDB and retrieve configuration informa-
                tions. ACDB provides these configuration in buffer referenced by
                uint8_t*. Even though the data type is know, ACDB doesn't main-
                tain the type information. It is maintained by device module in 
                csd_dev_device_info.h. It is over here were the burden of 
                types conversion and struct population done and there by abstra-
                cting out acdb access from device module.

  Dependencies: All the following files and related modules needs to be in sync
                @see csd_dev_device_info.h 
                @see acdb.h 
                @see wcd_types.h

  Copyright (c) 2008-13, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/b-family/src/csd_dev_acdb.c#2 $
  $DateTime: 2019/01/03 07:23:13 $
  $Author: pwbldsvc $
  $Change: 17964421 $
  $Revision: #2 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  11/15/16   rk        Abstracting OSAL layer
  02/06/14   vk        modified code for proper csd/acdb/apr error mapping
  10/07/13   vk        added support for AVSync feature
  02/25/13   suns      Device module unification.
  01/10/13   rp        Added SideTone IIR filter support
  08/06/12   up        9x25 Base
  07/24/12   up        PCM Support
  01/24/12   jp        Fix for device ID passing to acdb
  04/27/10   suns      Initial Version
==============================================================================*/

#include "csd_dev_acdb.h"
#include "acdb.h"
#include "DALSys.h"
#include "csd_debug_msg.h"
#include "csd_pmem.h"
#include "csd_dev_codec.h"
#include "csd_dev_oem_plugin_intf.h"
#include "wcd_intf.h"
#include "csd_dev_device_info.h"
#include "adsp_afe_service_commands.h"
#include "csd_os_dependencies.h"
#include "csd_error_map.h"

#define CSD_ACDB_DEFAULT_BUF_SIZE         4096
#define CSD_ACDB_INVALID_DEVICE_TYPE      0
/*==============================================================================
 * Refer to acdb.h
 * struct _AcdbDeviceInfoTargetSpecificRspType
   {
     uint16_t  ulPayloadMajorVersion;
     uint16_t  ulPayloadMinorVersion;
     uint8_t payload[payloadsize];
   }AcdbDeviceInfoTargetSpecificRspType
 *
 ================================================================================*/
/* Define Dummy Structure to get the CmdResp Payload data: as defined in acdb.h #2 */
typedef struct csd_acdb_i2s_type_info {
  uint32_t codec_path_id;
  uint32_t afe_port_id;
  uint32_t afe_channel_mode;
  uint32_t afe_mono_stereo; /*Mono(0), mono_right (1), mono_left (2) , stereo(3)*/
  uint32_t afe_word_sync_src_select; /*Word select source external (0), Word select source internal (1)*/
  uint32_t bits_per_sample_mask; /*This is not specific to Afe interface.*/
  uint32_t channel_config;
  uint32_t i2s_mclk_mode;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
  uint32_t afe_data_type; /*Linear pcm data(0), Non linear data(1), Linear pcm data packed in 60958(2), Non Linear data packed in 60958(3)*/
} csd_acdb_i2s_type_info_t;
#define CSD_ACDB_I2S_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_I2S_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_pcm_type_info {
  uint32_t afe_port_id;
  uint32_t afe_aux_mode;               /* PCM mode (0) AUX_PCM mode (1)*/
  uint32_t afe_sync_src_select;        /* Src_external(0) src_internal (1)*/
  uint32_t afe_frame_setting;          /* bits width per frame:
                                          8 bits   (0),
                                          16 bits  (1),
                                          32 bits  (2),
                                          64 bits  (3),
                                          128 bits (4),
                                          256 bits (5)*/
  uint32_t afe_quant_type;             /* Quant type:
                                          A-law no padding  (0),
                                          Mu-law no padding (1),
                                          linear no padding (2),
                                          A-law padding     (3),
                                          Mu-law padding    (4),
                                          linear padding    (5)*/
  uint32_t afe_data_out_enable_ctrl;   /* Output Data Enable Control:
                                          tri-state disable (0)
                                          tri-state enable  (1)*/
  uint32_t bits_per_sample_mask;
  uint32_t channel_config;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
  uint16_t slot_number_mapping[4];     /* specifies the outgoing\incoming
                                          PCM stream slot for the each channel
                                          [slot number: valid data 0 - 31]*/
  /* Major Version = 1, Minor Version = 1*/
  uint32_t mux_sel;                    /* specifies the MuxSelection
                                          I/O path on the lpaif interface:
                                          refered by AcdbPCMMuxType>*/
} csd_acdb_pcm_type_info_t;
#define CSD_ACDB_PCM_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_PCM_DEV_MINOR_VERSION 0x001

typedef struct csd_acdb_slimbus_type_info {
  uint32_t codec_path_id;
  uint32_t bits_per_sample_mask;
  uint32_t channel_config;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
  uint32_t afe_port_id;
  uint32_t afe_slimbux_dev_id;
  uint32_t afe_data_format;
  uint8_t shared_channel[AFE_PORT_MAX_AUDIO_CHAN_CNT];

} csd_acdb_slimbus_type_info_t;
#define CSD_ACDB_SLIMBUS_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_SLIMBUS_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_dmic_type_info {
  uint32_t afe_port_id;
  uint32_t afe_channel_mode;
  uint32_t bits_per_sample_mask; /*This is not specific to Afe interface.*/
  uint32_t channel_config;
  uint32_t master_clk_mode;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
} csd_acdb_dmic_type_info_t;
#define CSD_ACDB_DMIC_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_DMIC_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_hdmi_type_info {
  uint32_t afe_port_id;
  uint32_t afe_channel_mode;
  uint32_t afe_data_type;        /* linear data type     (0),
                                    non-linear data type (1)*/
  uint32_t bits_per_sample_mask; /* This is not specific to Afe interface.*/
  uint32_t channel_config;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
} csd_acdb_hdmi_type_info_t;
#define CSD_ACDB_HDMI_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_HDMI_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_virtual_type_info {
  uint32_t afe_port_id;
  uint32_t bits_per_sample_mask;  /* afe_bits_per_sample_mask;*/
  uint32_t channel_config;
  uint32_t afe_timing_mode;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
  uint32_t afe_data_type;         /* Linear pcm data (0),
                                     Non linear data (1),
                                     Linear pcm data packed in 60958 (2),
                                     Non Linear data packed in 60958 (3)*/
} csd_acdb_virtual_type_info_t;
#define CSD_ACDB_VIRTUAL_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_VIRTUAL_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_int_btfm_type_info {
  uint32_t afe_port_id;
  uint32_t bits_per_sample_mask;  /* This is not specific to Afe interface.*/
  uint32_t channel_config;
  uint32_t sample_rate_mask;
  uint32_t direction_mask;
} csd_acdb_int_btfm_type_info_t;
#define CSD_ACDB_BTFM_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_BTFM_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_rt_proxy_type_info {
  uint32_t sample_rate_mask;
  uint32_t channel_config;
  uint32_t direction_mask;
  uint32_t afe_port_id;
  uint32_t afe_bits_per_sample_mask;
} csd_acdb_rt_proxy_type_info_t;
#define CSD_ACDB_RTPROXY_DEV_MAJOR_VERSION 0x001
#define CSD_ACDB_RTPROXY_DEV_MINOR_VERSION 0x000

typedef struct csd_acdb_dev_info_target_resp_type {
  uint16_t major_version;
  uint16_t minor_version;
  union {
    csd_acdb_i2s_type_info_t i2s_info;
    csd_acdb_pcm_type_info_t pcm_info;
    csd_acdb_slimbus_type_info_t slimbus_info;
    csd_acdb_dmic_type_info_t dmic_info;
    csd_acdb_hdmi_type_info_t hdmi_info;
    csd_acdb_virtual_type_info_t virtual_info;
    csd_acdb_int_btfm_type_info_t int_btfm_info;
    csd_acdb_rt_proxy_type_info_t rt_proxy_info;
  } u;
} csd_acdb_dev_info_resp_t;

typedef struct csd_acdb_adie_sidetone_info
{
  uint32_t module_id;
  uint32_t param_id;
  uint16_t param_size; 
  uint16_t reserved;  
  uint8_t param_data[104];
}csd_acdb_adie_sidetone_info_t;

/* AV sync HW delay data strcuture */
typedef struct 
{
  uint32_t sample_rate;
  uint32_t delay_us;    /* delay in micro seconds*/
}csd_acdb_avsync_hw_delay_info_t;

/*==============================================================================
  Internal helper functions.
==============================================================================*/
static uint32_t acdb_buffer_length( int32_t interface_type )
{
  /* @see: csd_acdb_dev_info_target_resp_type for detail about the size */
  int32_t length = 2*sizeof(uint16_t); /* Major version + Minor version */

  switch (interface_type)
   {
     case ACDB_DEVICE_TYPE_I2S_PARAM:
       length += sizeof(csd_acdb_i2s_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_PCM_PARAM:
       length += sizeof(csd_acdb_pcm_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
       length += sizeof(csd_acdb_slimbus_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_DIGITAL_MIC_PARAM:
       length += sizeof(csd_acdb_dmic_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_HDMI_PARAM:
       length += sizeof(csd_acdb_hdmi_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_VIRTUAL_PARAM:
       length += sizeof(csd_acdb_virtual_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_INT_BTFM_PARAM:
       length += sizeof(csd_acdb_int_btfm_type_info_t);
       break;
     case ACDB_DEVICE_TYPE_RT_PROXY_PARAM:
       length += sizeof(csd_acdb_rt_proxy_type_info_t);
       break;
     default:
      length = CSD_ACDB_DEFAULT_BUF_SIZE;
      break;
  }
  return length;
}

static int32_t csd_acdb_set_dev_info_i2s_type( csd_acdb_dev_info_t *info,
                                               csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;

  if( (NULL == info) || (NULL == data) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_i2s_type: "
            "Bad Param info[%p],data[%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  if( CSD_EOK == rc)
  {
    /* Check for Major Version Support, Minor version not used */
    if ( CSD_ACDB_I2S_DEV_MAJOR_VERSION != data->major_version )
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_set_dev_info_i2s_type: "
                "I2S Params: MajorVersion=[0x%x], Not Supported!! ",
                     data->major_version);
    }
  }
  if( CSD_EOK == rc)
  {
    csd_acdb_dev_i2s_info_t* i2s_info = &info->u.i2s_dev_info;
    /* Common Params */
    info->bits_per_sample_mask = data->u.i2s_info.bits_per_sample_mask;
    info->afe_port_id = data->u.i2s_info.afe_port_id;
    info->channel_config = data->u.i2s_info.channel_config;
    /* Direction Mask translation */
    info->direction = data->u.i2s_info.direction_mask>>1;
    info->sample_rate_mask = data->u.i2s_info.sample_rate_mask;

    info->is_virtual = 0;

    /* I2S Device Specific Params */
    i2s_info->afe_channel_mode = data->u.i2s_info.afe_channel_mode;
    i2s_info->afe_mono_stereo = data->u.i2s_info.afe_mono_stereo;
    i2s_info->afe_word_sync_src_select = data->u.i2s_info.afe_word_sync_src_select;
    i2s_info->codec_path_id = data->u.i2s_info.codec_path_id;
    i2s_info->i2s_mclk_mode = data->u.i2s_info.i2s_mclk_mode;
    i2s_info->afe_data_type = data->u.i2s_info.afe_data_type;

    CSD_MSG(CSD_PRIO_LOW, "csd_acdb_set_dev_info_i2s_type: "
             "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
             info->bits_per_sample_mask, info->sample_rate_mask,
             info->direction);
    CSD_MSG(CSD_PRIO_LOW, "csd_acdb_set_dev_info_i2s_type: "
            "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
            info->channel_config, info->afe_port_id, info->is_virtual);
    CSD_MSG(CSD_PRIO_LOW, "afe_channel_mode=[0x%x], afe_mono_stereo=[0x%x], afe_word_sync_src_select=[0x%x]",
            info->u.i2s_dev_info.afe_channel_mode,
            info->u.i2s_dev_info.afe_mono_stereo,
            info->u.i2s_dev_info.afe_word_sync_src_select);
    CSD_MSG(CSD_PRIO_LOW, "csd_acdb_set_dev_info_i2s_type: "
            "codec_path_id=[0x%x], i2s_mclk_mode=[0x%x]",
            info->u.i2s_dev_info.codec_path_id, info->u.i2s_dev_info.i2s_mclk_mode);
    CSD_MSG(CSD_PRIO_LOW,"csd_acdb_set_dev_info_i2s_type: "
            "afe_data_type=[0x%x]",
            info->u.i2s_dev_info.afe_data_type);
    }

  return rc;
}

static int32_t csd_acdb_set_dev_info_pcm_type( csd_acdb_dev_info_t *info,
                                               csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;

  if ( (NULL != info) && (NULL != data) )
  {
    int32_t index = 0;
    /* Check for Major Version Support */
    if (CSD_ACDB_PCM_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
          "MajorVersion=[0x%x] Not Supported!", data->major_version);
    }
    else
    {
      /* Common Params */
      info->bits_per_sample_mask = data->u.pcm_info.bits_per_sample_mask;
      info->afe_port_id = data->u.pcm_info.afe_port_id;
      info->channel_config = data->u.pcm_info.channel_config;
      /* Direction Mask translation */
      info->direction = data->u.pcm_info.direction_mask>>1;
      info->sample_rate_mask = data->u.pcm_info.sample_rate_mask;

      info->is_virtual = 0;

      /* PCM Device Specific Params */
      info->u.pcm_dev_info.afe_aux_mode = data->u.pcm_info.afe_aux_mode;
      info->u.pcm_dev_info.afe_data_out_enable_ctrl = data->u.pcm_info.afe_data_out_enable_ctrl;
      info->u.pcm_dev_info.afe_frame_setting = data->u.pcm_info.afe_frame_setting;
      info->u.pcm_dev_info.afe_quant_type = data->u.pcm_info.afe_quant_type;
      info->u.pcm_dev_info.afe_sync_src_select = data->u.pcm_info.afe_sync_src_select;

      for (index = 0; index < 4; index++)
      {
        info->u.pcm_dev_info.afe_slot_number_mapping[index] = data->u.pcm_info.slot_number_mapping[index];
      }
      /* Added MuxSelection from Version 1.1 */
      if (CSD_ACDB_PCM_DEV_MINOR_VERSION <= data->minor_version)
      {
        info->u.pcm_dev_info.mux_sel = data->u.pcm_info.mux_sel;
      }
      else
      {
        CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
            "MuxSel is not available in ACDB with MinorVersion=[0x%x]. "
            "Setting to [1] for Primary and [2] for Secondary",
            data->minor_version );
        if ( (AFE_PORT_ID_PRIMARY_PCM_RX == info->afe_port_id) ||
             (AFE_PORT_ID_PRIMARY_PCM_TX == info->afe_port_id) )
        {
          info->u.pcm_dev_info.mux_sel = 0x01;
        }
        else
        {
          info->u.pcm_dev_info.mux_sel = 0x02;
        }
      }
/*
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
          "MajorVersion=[0x%x], MinorVersion=[0x%x]",
          data->major_version, data->minor_version);
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
          "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
          info->bits_per_sample_mask, info->sample_rate_mask, info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
          "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
          info->channel_config,info->afe_port_id, info->is_virtual );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params:"
          " afe_aux_mode=[0x%x], afe_data_out_enable_ctrl=[0x%x], afe_frame_setting=[0x%x]",
          info->u.pcm_dev_info.afe_aux_mode, info->u.pcm_dev_info.afe_data_out_enable_ctrl,
          info->u.pcm_dev_info.afe_frame_setting );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
          "afe_quant_type=[0x%x], afe_sync_src_select=[0x%x], mux_sel=[0x%x]",
          info->u.pcm_dev_info.afe_quant_type, info->u.pcm_dev_info.afe_sync_src_select,
          info->u.pcm_dev_info.mux_sel);
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_pcm_type: PCM Params: "
          "SlNum[0]=[0x%x], SlNum[1]=[0x%x], SlNum[2]=[0x%x], SlNum[3]=[0x%x]",
          info->u.pcm_dev_info.afe_slot_number_mapping[0], info->u.pcm_dev_info.afe_slot_number_mapping[1],
          info->u.pcm_dev_info.afe_slot_number_mapping[2], info->u.pcm_dev_info.afe_slot_number_mapping[3] );
*/
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_pcm_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  return rc;
}

static int32_t csd_acdb_set_dev_info_slimbus_type( csd_acdb_dev_info_t *info,
                                                   csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;
/*
  if (NULL != info && NULL != data)
  {

    // Check for Major Version Support
    if (CSD_ACDB_SLIMBUS_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_slimbus_type: "
               "SLIMBUS Params: MajorVersion=[0x%x] Not Supported!",
          data->major_version);
    }
    else
    {
      // Common Params
      info->afe_port_id = data->u.slimbus_info.afe_port_id;
      info->bits_per_sample_mask = data->u.slimbus_info.bits_per_sample_mask;
      // Direction Mask translation
      info->direction = data->u.slimbus_info.direction_mask>>1;
      info->sample_rate_mask = data->u.slimbus_info.sample_rate_mask;
      info->channel_config = data->u.slimbus_info.channel_config;

      info->is_virtual = 0;

      // Slimbus Device Specific Params
      info->u.slimbus_dev_info.codec_path_id = data->u.slimbus_info.codec_path_id;
      info->u.slimbus_dev_info.afe_slimbus_dev_id = data->u.slimbus_info.afe_slimbux_dev_id;
      info->u.slimbus_dev_info.afe_data_format = data->u.slimbus_info.afe_data_format;


      // Memory set and copy the Shared channel entries
      csd_memset( &info->u.slimbus_dev_info.afe_shared_channel[0], 0,
                     (sizeof(uint8_t) * AFE_PORT_MAX_AUDIO_CHAN_CNT) );

      csd_memscpy( &info->u.slimbus_dev_info.afe_shared_channel[0],
                   sizeof(info->u.slimbus_dev_info.afe_shared_channel),
                   data->u.slimbus_info.shared_channel,
                   (sizeof(uint8_t) * AFE_PORT_MAX_AUDIO_CHAN_CNT) );

      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_slimbus_type: SLIMBUS Params: "
          "MajorVersion=[0x%x], MinorVersion=[0x%x]",
          data->major_version, data->minor_version );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_slimbus_type: SLIMBUS Params: "
          "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
          info->bits_per_sample_mask, info->sample_rate_mask, info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_slimbus_type: SLIMBUS Params: "
          "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
          info->channel_config, info->afe_port_id, info->is_virtual );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_slimbus_type: SLIMBUS Params: "
          "CodecPathId=[0x%x], SbDevId=[0x%x], DataFormat=[0x%x]",
          info->u.slimbus_dev_info.codec_path_id, info->u.slimbus_dev_info.afe_slimbus_dev_id,
          info->u.slimbus_dev_info.afe_data_format);
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_slimbus_type: SLIMBUS Params: "
          "ShCh[0]=[0x%x], ShCh[1]=[0x%x], ShCh[2]=[0x%x]",
          info->u.slimbus_dev_info.afe_shared_channel[0],
          info->u.slimbus_dev_info.afe_shared_channel[1],
          info->u.slimbus_dev_info.afe_shared_channel[2]);

    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_slimbus_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  */
  return rc;
}
static int32_t csd_acdb_set_dev_info_dmic_type( csd_acdb_dev_info_t *info,
                                                csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;
  if (NULL != info && NULL != data)
  {
    /* Check for Major Version Support */
    if (CSD_ACDB_DMIC_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_dmic_type: "
          "DMIC Params: MajorVersion=[0x%x], Not Supported!",data->major_version);
    }
    else
    {
      /* Common Params */
      info->bits_per_sample_mask = data->u.dmic_info.bits_per_sample_mask;
      info->afe_port_id = data->u.dmic_info.afe_port_id;
      info->channel_config = data->u.dmic_info.channel_config;
      /* Direction Mask translation */
      info->direction = data->u.dmic_info.direction_mask>>1;
      info->sample_rate_mask = data->u.dmic_info.sample_rate_mask;

      info->is_virtual = 0;

      /* DMIC Device Specific Params */
      info->u.dmic_dev_info.afe_channel_mode = data->u.dmic_info.afe_channel_mode;
      info->u.dmic_dev_info.master_clk_mode = data->u.dmic_info.master_clk_mode;

  //    CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_dmic_type: DMIC Params: "
  //        "MajorVersion=[0x%x], MinorVersion=[0x%x]",
  //        data->major_version, data->minor_version );
  //    CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_dmic_type: DMIC Params: "
  //        "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
  //        info->bits_per_sample_mask, info->sample_rate_mask, info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_dmic_type: DMIC Params: "
          "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
          info->channel_config,info->afe_port_id, info->is_virtual );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_dmic_type: DMIC Params: "
          "ChannelMode=[0x%x], MCLK=[0x%x]",
          info->u.dmic_dev_info.afe_channel_mode,
          info->u.dmic_dev_info.master_clk_mode );
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_dmic_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  return rc;
}
static int32_t csd_acdb_set_dev_info_hdmi_type( csd_acdb_dev_info_t *info,
                                                csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;
  if (NULL != info && NULL != data)
  {
    /* Check for Major Version Support */
    if (CSD_ACDB_HDMI_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_hdmi_type: "
              "HDMI Params: MajorVersion=[0x%x], Not Supported!! ",data->major_version);
    }
    else
    {
      /* Common Params */
      info->bits_per_sample_mask = data->u.hdmi_info.bits_per_sample_mask;
      info->afe_port_id = data->u.hdmi_info.afe_port_id;
      info->channel_config = data->u.hdmi_info.channel_config;
      /* Direction Mask translation */
      info->direction = data->u.hdmi_info.direction_mask>>1;
      info->sample_rate_mask = data->u.hdmi_info.sample_rate_mask;

      info->is_virtual = 0;

      /* HDMI Device Specific Params */
      info->u.hdmi_dev_info.afe_channel_mode = data->u.hdmi_info.afe_channel_mode;
      info->u.hdmi_dev_info.afe_data_type = data->u.hdmi_info.afe_data_type;

/*
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_hdmi_type: HDMI Params: "
          "MajorVersion=[0x%x], MinorVersion=[0x%x]",
          data->major_version, data->minor_version );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_hdmi_type: HDMI Params: "
          "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
          info->bits_per_sample_mask, info->sample_rate_mask, info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_hdmi_type: HDMI Params: "
          "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
          info->channel_config,info->afe_port_id, info->is_virtual );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_hdmi_type: HDMI Params: "
          "ChannelMode=[0x%x], DataType=[0x%x]",
          info->u.hdmi_dev_info.afe_channel_mode, info->u.hdmi_dev_info.afe_data_type );
*/
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_hdmi_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  return rc;
}

static int32_t csd_acdb_set_dev_info_virtual_type( csd_acdb_dev_info_t *info,
                                                   csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;
  if (NULL != info && NULL != data)
  {
    /* Check for Major Version Support */
    if (CSD_ACDB_VIRTUAL_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_virtual_type: VIRTUAL Params: "
          "MajorVersion=[0x%x], Not Supported!! ",data->major_version);
    }
    else
    {
      /* Common Params */
      info->bits_per_sample_mask = data->u.virtual_info.bits_per_sample_mask;
      info->afe_port_id = data->u.virtual_info.afe_port_id;
      info->channel_config = data->u.virtual_info.channel_config;
      /* Direction Mask translation */
      info->direction = data->u.virtual_info.direction_mask>>1;
      info->sample_rate_mask = data->u.virtual_info.sample_rate_mask;

      info->is_virtual = 1;

      /* Virtual Device Specific Params */
      info->u.virtual_dev_info.afe_timing_mode = data->u.virtual_info.afe_timing_mode;
      info->u.virtual_dev_info.afe_data_type = data->u.virtual_info.afe_data_type;

/*
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_virtual_type: VIRTUAL Params: "
          "MajorVersion=[0x%x], MinorVersion=[0x%x]",
          data->major_version, data->minor_version );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_virtual_type: VIRTUAL Params: "
          "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
          info->bits_per_sample_mask, info->sample_rate_mask, info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_virtual_type: VIRTUAL Params: "
          "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
          info->channel_config,info->afe_port_id, info->is_virtual );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_virtual_type: VIRTUAL Params: "
          "TimingMode=[0x%x], DataFormat=[0x%x]",
          info->u.virtual_dev_info.afe_timing_mode, info->u.virtual_dev_info.afe_data_type );
*/
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_virtual_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  return rc;
}

static int32_t csd_acdb_set_dev_info_int_btfm_type( csd_acdb_dev_info_t *info,
                                                    csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;
  if (NULL != info && NULL != data)
  {
    /* Check for Major Version Support */
    if (CSD_ACDB_BTFM_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_int_btfm_type: BT_FM Params: "
          "MajorVersion=[0x%x], Not Supported!",data->major_version);
    }
    else
    {
      /* Common Params */
      info->bits_per_sample_mask = data->u.int_btfm_info.bits_per_sample_mask;
      info->afe_port_id = data->u.int_btfm_info.afe_port_id;
      info->channel_config = data->u.int_btfm_info.channel_config;
      /* Direction Mask translation */
      info->direction = data->u.int_btfm_info.direction_mask>>1;
      info->sample_rate_mask = data->u.int_btfm_info.sample_rate_mask;

      info->is_virtual = 0;

/*
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_int_btfm_type: BT_FM Params: "
          "MajorVersion=[0x%x], MinorVersion=[0x%x]",
          data->major_version, data->minor_version );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_int_btfm_type: BT_FM Params: "
          "Bps=[0x%x], SRM=[0x%x], DirectionMask=[0x%x]",
          info->bits_per_sample_mask, info->sample_rate_mask, info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_int_btfm_type: BT_FM Params: "
          "ChannelConfig=[0x%x], AfePortId=[0x%x], isVirtual=[0x%x]",
          info->channel_config,info->afe_port_id, info->is_virtual );
*/
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_int_btfm_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  return rc;
}

static int32_t csd_acdb_set_dev_info_rt_proxy_type( csd_acdb_dev_info_t *info,
                                                    csd_acdb_dev_info_resp_t *data )
{
  int32_t rc = CSD_EOK;

  /*
  if (NULL != info && NULL != data)
  {
    // Check for Major Version Support
    if (CSD_ACDB_RTPROXY_DEV_MAJOR_VERSION != data->major_version)
    {
      rc = CSD_EUNSUPPORTED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_set_dev_info_rt_proxy_type: "
              "RT_PROXY Params: MajorVersion=[0x%x], "
               "Not Supported!! ",data->major_version);
    }
    else
    {
      info->bits_per_sample_mask = data->u.rt_proxy_info.afe_bits_per_sample_mask;
      info->afe_port_id = data->u.rt_proxy_info.afe_port_id;
      info->channel_config = data->u.rt_proxy_info.channel_config;
      // Direction Mask translation
      info->direction = data->u.rt_proxy_info.direction_mask>>1;
      info->sample_rate_mask = data->u.rt_proxy_info.sample_rate_mask;

      info->is_virtual = 0;

      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_rt_proxy_type: "
               "RT_PROXY Params: MajorVersion=[0x%x], "
               "MinorVersion=[0x%x]",
               data->major_version, data->minor_version );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_rt_proxy_type: "
               "RT_PROXY Params: Bps=[0x%x], SRM=[0x%x], "
               "DirectionMask=[0x%x]",
               info->bits_per_sample_mask, info->sample_rate_mask,
               info->direction );
      CSD_MSG( CSD_PRIO_LOW, "csd_acdb_set_dev_info_rt_proxy_type: "
               "RT_PROXY Params: ChannelConfig=[0x%x], "
               "AfePortId=[0x%x], isVirtual=[0x%x]",
               info->channel_config,info->afe_port_id, info->is_virtual );
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_set_dev_info_rt_proxy_type: "
            "Null Args: info[0x%p], data[0x%p]", (void*)info, (void*)data);
    rc = CSD_EBADPARAM;
  }
  */
  return rc;
}


/*
 * Prerequisite: the input param "csd_acdb_dev_info *info" should
 * have valid DeviceType Set (using the get Common Dev_Info)
 */
static int32_t csd_acdb_get_dev_specific_info( uint32_t dev_id, csd_acdb_dev_info_t *info )
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbDeviceInfoTargetSpecificCmdType acdb_cmd = { 0, };
  AcdbQueryResponseType acdb_cmd_response = { 0, };
  csd_acdb_dev_info_resp_t *acdb_response_data = NULL;
  uint32_t expected_buf_length = 0;
  uint32_t acdb_command_id = ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO;

  if (NULL == info)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_specific_info: NULL Param");
    rc = CSD_EBADPARAM;
  }
  else
  {
    expected_buf_length = acdb_buffer_length(info->interface_type);

    /* Allocate Memory */
	csd_malloc(expected_buf_length, (void**)&acdb_response_data);
    if (acdb_response_data == NULL )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_specific_info: "
              "csd_acdb_get_dev_info_target_type() "
              "failed to allocate memory size of (%d)!", expected_buf_length);
      rc = CSD_ENORESOURCE;
    }
    else
    {
      /* Initialize acdb cmd and response Data */
      csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
      csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

      /* Setup the ACDB Command */
      acdb_cmd.nDeviceId = dev_id;
      acdb_cmd.nDeviceType = info->interface_type;
      acdb_cmd.nBufferLength = expected_buf_length;
      acdb_cmd.pBufferPointer = (void*) acdb_response_data;

      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_get_dev_specific_info: "
               "Invoking ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO,"
               "DeviceID[0x%x], DeviceType[0x%x]",
               acdb_cmd.nDeviceId, acdb_cmd.nDeviceType);

      /* query acdb */
      acdb_rc = acdb_ioctl(acdb_command_id, (const byte*) &acdb_cmd, sizeof(acdb_cmd),
          (uint8_t *) &acdb_cmd_response, sizeof(acdb_cmd_response));
      if (ACDB_SUCCESS != acdb_rc)
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_dev_specific_info: "
                 "ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO"
                 "failed, acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd(acdb_rc);
      }
      else
      {
        /* Expected size compensation based on Minor Versioning */
        if((ACDB_DEVICE_TYPE_PCM_PARAM == info->interface_type) &&
           (CSD_ACDB_PCM_DEV_MAJOR_VERSION == acdb_response_data->major_version) &&
           (CSD_ACDB_PCM_DEV_MINOR_VERSION > acdb_response_data->minor_version) )
        {
          expected_buf_length -= sizeof(uint32_t); /*Mux Sel not present*/
        }
        /* Expected size check */
        if(acdb_cmd_response.nBytesUsedInBuffer != expected_buf_length )
        {

          CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_dev_specific_info: "
                   "Return buffer size doesn't match "
                   "(ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO)"
                   "received[%d], expected[%d]",
                   acdb_cmd_response.nBytesUsedInBuffer,
                   expected_buf_length);
          rc = CSD_EBADPARAM;
        }
      }
    }
  }
  if(CSD_EOK == rc && ACDB_SUCCESS == acdb_rc)
  {
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_dev_specific_info: "
            "ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO success,"  
            " returned Bytes[%d],MajorVersion[0x%x], MajorVersion[0x%x],"
            " DeviceType[0x%x]", expected_buf_length, 
            acdb_response_data->major_version,
            acdb_response_data->minor_version, info->interface_type);

    if (acdb_cmd_response.nBytesUsedInBuffer)
    {
      rc = CSD_EOK;
      switch (info->interface_type)
      {
        case ACDB_DEVICE_TYPE_I2S_PARAM:
          rc = csd_acdb_set_dev_info_i2s_type(info, acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_PCM_PARAM:
          rc = csd_acdb_set_dev_info_pcm_type(info, acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
          rc = csd_acdb_set_dev_info_slimbus_type(info,acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_DIGITAL_MIC_PARAM:
          rc = csd_acdb_set_dev_info_dmic_type(info, acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_HDMI_PARAM:
          rc = csd_acdb_set_dev_info_hdmi_type(info, acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_VIRTUAL_PARAM:
          rc = csd_acdb_set_dev_info_virtual_type(info, acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_INT_BTFM_PARAM:
          rc = csd_acdb_set_dev_info_int_btfm_type(info, acdb_response_data);
          break;
        case ACDB_DEVICE_TYPE_RT_PROXY_PARAM:
          rc = csd_acdb_set_dev_info_rt_proxy_type(info, acdb_response_data);
          break;
        default:
          rc = CSD_EFAILED;
          CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_get_dev_specific_info: "
                   "ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO "
                   "for Unknown DeviceType[0x%x]!", info->interface_type);
          break;
      }
    }
    else
    {
      rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_get_dev_specific_info: "
          "ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO "
          "Input DeviceType[0x%x] Doesn't Match Resp[0x%x]!",
          info->interface_type,info->interface_type);
    }
  }
  if(acdb_response_data)
  {
    /* Free the Buffer */
    csd_free(acdb_response_data);
  }
  return rc;
}

/*==============================================================================
 * Refer to acdb.h
 * struct _AcdbDeviceInfoCommonRspType
   {
     uint32_t headerVersion: The version of the common device info type structure format
               ACDB_COMMON_DEVICE_INFO_HDR_PARAM - 0x000112A7
               - First Common device info header version ID
     uint8_t payload[payloadsize]; - payloadsize is identified by the common device info
             structure size.
   } AcdbDeviceInfoCommonRspType
 ================================================================================*/
static int32_t csd_acdb_get_dev_common_info( uint32_t dev_id, uint32_t *interface_type )
{
  /* Define Dummy Structure to get the CmdResp */
  typedef struct {
    uint32_t header;
    uint32_t device_type;
  } csd_acdb_dev_info_cmn_resp_t;

  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;

  AcdbDeviceInfoCmnCmdType acdb_cmd = { 0, };
  AcdbQueryResponseType acdb_cmd_response = { 0, };
  csd_acdb_dev_info_cmn_resp_t cmd_resp_data = { 0, 0 };
  csd_acdb_dev_info_cmn_resp_t *acdb_buffer = &cmd_resp_data;
  uint32_t bytes_used_in_buffer = 0;
  uint32_t acdb_command_id = ACDB_CMD_GET_COMMON_DEVICE_INFO;

  if (NULL == interface_type)
  {
    CSD_MSG(CSD_PRIO_ERROR,"NULL PARAM");
    rc = CSD_EBADPARAM;
  }
  else
  {
    /* Initialize acdb cmd and response Data */
    csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
    csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

    /* Setup the ACDB Command */
    acdb_cmd.nDeviceId = dev_id;
    acdb_cmd.nBufferLength = sizeof(csd_acdb_dev_info_cmn_resp_t);
    acdb_cmd.nBufferPointer = (void*) acdb_buffer;

    CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_get_dev_common_info: "
             "Invoking ACDB_CMD_GET_COMMON_DEVICE_INFO,"
             " DeviceId[0x%x]", acdb_cmd.nDeviceId );
    /* query acdb */
    acdb_rc = acdb_ioctl(acdb_command_id,
                         (const byte*) &acdb_cmd, sizeof(acdb_cmd),
                         (uint8_t *) &acdb_cmd_response, sizeof(acdb_cmd_response));

    if (ACDB_SUCCESS != acdb_rc)
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_dev_common_info: "
                 "ACDB_CMD_GET_COMMON_DEVICE_INFO failed,"
                 "acdb_rc[%d]", acdb_rc );
      rc = csd_map_acdb_error_to_csd(acdb_rc);
    }
    else
    {
      bytes_used_in_buffer = acdb_cmd_response.nBytesUsedInBuffer;
      *interface_type = cmd_resp_data.device_type;

      CSD_MSG( CSD_PRIO_MEDIUM,"csd_acdb_get_dev_common_info: "
               "ACDB_CMD_GET_COMMON_DEVICE_INFO success,"
               "returned Bytes[%d], HeaderVersion[0x%x], DeviceType[0x%x]",
               bytes_used_in_buffer, cmd_resp_data.header, *interface_type);
    }
  } /*end if (NULL == info) */
  return rc;
}

static int32_t csd_acdb_get_dev_info( uint32_t dev_id, csd_acdb_dev_info_t *info )
{
  int32_t rc = CSD_EOK;

  if (NULL != info)
  {
    /* Query for interface type */
    rc = csd_acdb_get_dev_common_info(dev_id, &info->interface_type);
    if (rc == CSD_EOK)
    {
      rc = csd_acdb_get_dev_specific_info(dev_id, info);
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_dev_info: "
              "For DeviceId=[%d] Failed With Err=[0x%x]!!",
              dev_id, rc);
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_info: "
            "Failed due to NULL Argument!!");
    rc = CSD_EBADPARAM;
  }

  return rc;
}

static int32_t csd_acdb_get_device_pair(uint32_t acdb_pair_type,
                                        uint32_t rx_dev_id,
                                        uint32_t *tx_dev_id)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  /* Sanity check on the input params */
  if((NULL != tx_dev_id))
  {
    *tx_dev_id = CSD_DEV_ID_INVALID;
    if((CSD_DEV_ID_INVALID != rx_dev_id))
    {
      AcdbDevicePairListCmdType acdb_cmd = {0};
      AcdbQueryResponseType acdb_cmd_response = {0,};
      uint32_t *acdb_buffer = NULL;
      uint32_t acdb_command_id = ACDB_CMD_GET_DEVICE_PAIR_LIST;
             
      csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE,(void**)&acdb_buffer);  
      if ( NULL == acdb_buffer )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_device_pair: "
                "failed to allocate memory size of (%d)!",
                CSD_ACDB_DEFAULT_BUF_SIZE);
        return CSD_ENORESOURCE; 
      }
      
      csd_memset(acdb_buffer, 0, CSD_ACDB_DEFAULT_BUF_SIZE);
      
      /* Initialize acdb cmd and response Data */
      csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
      csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

      /* Setup the ACDB Command */
      acdb_cmd.nDevPairType   = acdb_pair_type;
      acdb_cmd.nDeviceID      = rx_dev_id;
      acdb_cmd.nBufferLength  = CSD_ACDB_DEFAULT_BUF_SIZE;
      acdb_cmd.pBufferPointer = (void*)acdb_buffer;
  
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_get_device_pair: "
                "Invoking ACDB_CMD_GET_DEVICE_PAIR_LIST,"
                 " rx_dev_id[0x%x], DevicePairType[0x%x]",
                 acdb_cmd.nDeviceID, acdb_cmd.nDevPairType );
      
        /* query acdb */
      acdb_rc = acdb_ioctl(acdb_command_id,(const byte*)&acdb_cmd, 
              sizeof(acdb_cmd),(uint8_t *)&acdb_cmd_response, 
              sizeof(acdb_cmd_response));
      if( (ACDB_SUCCESS != acdb_rc) ||
          (acdb_cmd_response.nBytesUsedInBuffer < (sizeof(csd_acdb_dev_pair_list_type_t))) )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_device_pair: "
                 "ACDB_CMD_GET_DEVICE_PAIR_LIST failed,"
                 " acdb_rc[%d], RespBufLen[%d]",
                 acdb_rc, acdb_cmd_response.nBytesUsedInBuffer );
        rc = csd_map_acdb_error_to_csd(acdb_rc); 
      }
      else
      {
        csd_acdb_dev_pair_list_type_t DevList = {0,};
        DevList.noofdevs = *(acdb_buffer);
        if(DevList.noofdevs > 0)
        {
          DevList.devList = (uint32_t*)(((uint8_t*)acdb_buffer) + sizeof(uint32_t));
          *tx_dev_id = DevList.devList[0];
          rc = CSD_EOK;
          CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_device_pair: "
                  "rx_dev_id[%d] mapped with nDevs=[%d]"
                  " First tx_dev_id= [%d]",
                  rx_dev_id, DevList.noofdevs, *tx_dev_id);
        }
        else
        {
          rc = CSD_EFAILED;
          CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_device_pair:" 
               "Error!! Empty List in the RespBufLen=[%d]!",
               DevList.noofdevs );
        }
      }
      if( NULL != acdb_buffer )
      {
        /* Free allocated space */
        csd_free(acdb_buffer);
      }
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_get_device_pair: "
               "Invalid LocalDeviceID!! Failed to retrieve "
               "mapping ID for rx_dev_id[0x%x]!", rx_dev_id);
      rc = CSD_EBADPARAM;
    }
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_get_device_pair: "
             "NULL Remote Mapping Device !!  Failed to retrieve"
             " mapping ID for rx_dev_id[0x%x]!", rx_dev_id);
    rc = CSD_EBADPARAM;
  }
  
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_acdb_get_dev_info

  DESCRIPTION   Retrieve device info from ACDB based on device id passed. ACDB
                expects arguments to be passed in a certain format using acdb_
                ioctl. As part of the ACDB argument, a buffer pointer for stori-
                ng the results are passed. Space for this buffer must be pre-
                allocated.

  DEPENDENCIES  ACDB related structures
                @see acdb.h
                @see csd_dev_device_info.h

  PARAMETERS    @param dev_id : device id specific to the queried device
                @param  info   : pointer to allocated space for result

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_acdb_get_dev_info( uint32_t dev_id, void *in )
{
  int32_t rc = CSD_EOK;
  csd_acdb_dev_info_t *info = (csd_acdb_dev_info_t*) in;

  rc = csd_acdb_get_dev_info(dev_id, info);

  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_aanc_device_pair

  DESCRIPTION   Get AANC device paired TX for RX

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    rx_dev_id : input rx device id
                tx_dev_id : ouput tx device id

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_aanc_device_pair(uint32_t rx_dev_id, uint32_t *tx_dev_id)
{
  CSD_MSG( CSD_PRIO_MEDIUM,"csd_acdb_get_aanc_device_pair: "
           "ACDB Query ACDB_CMD_GET_DEVICE_PAIR_LIST for "
           "AANC Rx Dev_Id=[%d]!", rx_dev_id );
  return csd_acdb_get_device_pair(ACDB_ADAPTIVE_ANC_RX2TX_LIST,
                                  rx_dev_id,tx_dev_id);
}

/*=============================================================================

  FUNCTION      csd_acdb_get_afe_aanc_enable

  DESCRIPTION   TODO

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    tx_dev_id : TODO
                aanc_enable : TODO
                buf_size : TODO

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_afe_aanc_enable(uint32_t tx_dev_id, void *aanc_enable,
                                     uint32_t *buf_size)
{
  int32_t rc = CSD_EOK;

  /*
  int32_t acdb_rc = ACDB_SUCCESS;

  if( (NULL != aanc_enable) && (NULL != buf_size) )
  {
    AcdbAANCConfigDataCmdType acdb_cmd      = {0,};
    AcdbQueryResponseType acdb_cmd_response = {0,};
    
    csd_memset(&acdb_cmd, 0, sizeof(AcdbAANCConfigDataCmdType));
    csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

    acdb_cmd.nTxDeviceId    = tx_dev_id;
    acdb_cmd.nModuleId      = AFE_MODULE_AANC;
    acdb_cmd.nParamId       = AFE_PARAM_ID_ENABLE;
    acdb_cmd.nBufferPointer = (uint8_t *)aanc_enable;
    acdb_cmd.nBufferLength  = *buf_size;

    CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_get_afe_aanc_enable: "
             "Invoking ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA"
             " tx_dev_id[0x%x], ModuleID[0x%x], ParamID[0x%x]",
             acdb_cmd.nTxDeviceId, acdb_cmd.nModuleId, acdb_cmd.nParamId );

    acdb_rc = acdb_ioctl(ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA,
            (const byte*) &acdb_cmd, sizeof(AcdbAANCConfigDataCmdType), 
            (uint8_t *) &acdb_cmd_response, sizeof(AcdbQueryResponseType));
    if( ACDB_SUCCESS != acdb_rc ) 
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_afe_aanc_enable: "
                "ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA failed," 
                " acdb_rc[%d], nBytesUsedInBuffer[%d]",
              acdb_rc,acdb_cmd_response.nBytesUsedInBuffer);
      rc = csd_map_acdb_error_to_csd(acdb_rc); 
    }
    else
    {
      rc = CSD_EOK;
      *buf_size = acdb_cmd_response.nBytesUsedInBuffer;
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_afe_aanc_enable: "
                "ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA success,"
                " got the AANC enable info of Size[%d]!", *buf_size);
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_afe_aanc_enable: "
            "Invalid input params! rc[0x%x], "
            "aanc_enable[%p], buf_size[%p]", rc, (void*)aanc_enable, (void*)buf_size);
  }
  */
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_afe_aanc_cfg

  DESCRIPTION   TODO

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    tx_dev_id : TODO
                aanc_cfg1 : TODO
                cfg1_buf_size : TODO
                aanc_cfg2 : TODO
                cfg2_buf_size : TODO

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_afe_aanc_cfg(uint32_t tx_dev_id, void *aanc_cfg1,
                                  uint32_t *cfg1_buf_size, void *aanc_cfg2,
                                  uint32_t *cfg2_buf_size)
{
  int32_t rc = CSD_EOK;

  /*
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbAANCConfigDataCmdType acdb_cmd    = {0,};
  AcdbQueryResponseType acdb_cmd_response = {0,};
  
  if( (NULL != aanc_cfg1) && (NULL != cfg1_buf_size) )
  {
    csd_memset(&acdb_cmd, 0, sizeof(AcdbAANCConfigDataCmdType));
    csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

    acdb_cmd.nTxDeviceId  = tx_dev_id;
    acdb_cmd.nModuleId    = AFE_MODULE_AANC;
    acdb_cmd.nParamId     = AFE_PARAM_ID_AANC_CFG1;
    acdb_cmd.nBufferPointer = (uint8_t *)aanc_cfg1;
    acdb_cmd.nBufferLength  = *cfg1_buf_size;
    
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_afe_aanc_cfg: "
            "Invoking ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA for"
            "AANC CFG1 data, tx_dev_id[0x%x], ModuleID[0x%x], ParamID[0x%x]",
             acdb_cmd.nTxDeviceId, acdb_cmd.nModuleId, acdb_cmd.nParamId );

    acdb_rc = acdb_ioctl(ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA,
                    (const byte*) &acdb_cmd, sizeof(acdb_cmd),
                    (uint8_t *) &acdb_cmd_response, sizeof(acdb_cmd_response));
    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_afe_aanc_cfg: "
                "ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA failed,"
                "acdb_rc[%d], nBytesUsedInBuffer[%d]",
                acdb_rc, acdb_cmd_response.nBytesUsedInBuffer);
      rc = csd_map_acdb_error_to_csd(acdb_rc); 
    }
    else
    {
      rc = CSD_EOK;
      *cfg1_buf_size = acdb_cmd_response.nBytesUsedInBuffer;
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_afe_aanc_cfg: "
                "ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA success,"  
                "got the AANC CFG1 info data of size[%d]!", *cfg1_buf_size);
    }
  }
  
  if( (CSD_EOK == rc) && (NULL != aanc_cfg2) && (NULL != cfg2_buf_size) )
  {
    
    csd_memset(&acdb_cmd, 0, sizeof(AcdbAANCConfigDataCmdType));
    csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

    acdb_cmd.nTxDeviceId  = tx_dev_id;
    acdb_cmd.nModuleId    = AFE_MODULE_AANC;
    acdb_cmd.nParamId     = AFE_PARAM_ID_AANC_CFG2 ;
    acdb_cmd.nBufferPointer = (uint8_t*)aanc_cfg2;
    acdb_cmd.nBufferLength  = *cfg2_buf_size;
    
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_afe_aanc_cfg: "
             "Invoking ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA for"
             "AANC CFG2 data, tx_dev_id[0x%x], ModuleID[0x%x], ParamID[0x%x]",
             acdb_cmd.nTxDeviceId, acdb_cmd.nModuleId, acdb_cmd.nParamId );

    acdb_rc = acdb_ioctl(ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA,
                    (const byte*) &acdb_cmd, sizeof(acdb_cmd),
                    (uint8_t *) &acdb_cmd_response, sizeof(acdb_cmd_response));
    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_afe_aanc_cfg: "
              "ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA failed,"
              "acdb_rc[%d], nBytesUsedInBuffer[%d]",
               acdb_rc, acdb_cmd_response.nBytesUsedInBuffer);
      rc = csd_map_acdb_error_to_csd(acdb_rc); 
    }
    else
    {
      rc = CSD_EOK;
      *cfg2_buf_size = acdb_cmd_response.nBytesUsedInBuffer;
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_afe_aanc_cfg: "
                "ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA success,"  
                "got the AANC CFG2 info data of size[%d]!", *cfg2_buf_size);
    }
  }*/
  return rc;
}
/*=============================================================================

  FUNCTION      csd_acdb_get_fb_spkr_prot_device_pair

  DESCRIPTION   gets fb speaker protection device pair

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    rx_dev_id : TODO
                tx_dev_id : TODO

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
/*int32_t csd_acdb_get_fb_spkr_prot_device_pair(uint32_t rx_dev_id,
                                              uint32_t *tx_dev_id)
{
  //CSD_MSG( CSD_PRIO_MEDIUM,"csd_acdb_get_fb_spkr_prot_device_pair: "
  //         "ACDB Query ACDB_CMD_GET_DEVICE_PAIR_LIST for "
  //         "FB SPKR Rx Dev_Id=[%d]!", rx_dev_id );
  return csd_acdb_get_device_pair(ACDB_FEEDBACK_SPEAKERPROTECTION_RX2TX_LIST,
                                  rx_dev_id,tx_dev_id);
}*/

/*=============================================================================

  FUNCTION      csd_acdb_is_dev_feature_enabled

  DESCRIPTION   gets devices that support this feature/capability from ACDB

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    feature_id : TODO
                p_cap_dev_list : Buffer pointer to be populated with data
                buf_size : Allocated buffer size
                buf_size_used : Amount of buffer size used

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_devices_of_feature(uint32_t feature_id,
                                        uint8_t* p_cap_dev_list,
                                        uint32_t buf_size,
                                        uint32_t* buf_size_used)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;

  AcdbFeatureSupportedDevListCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  void *acdb_buffer = NULL;
  uint32_t acdb_command_id = ACDB_CMD_GET_FEATURE_SUPPORTED_DEVICE_LIST;

  if(p_cap_dev_list == NULL || buf_size_used == NULL)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_devices_of_feature: "
            "Null args: p_cap_dev_list[0x%p], actual_buf_size[0x%p]",
            (void*)p_cap_dev_list, (void*)buf_size_used);
    return CSD_EBADPARAM;
  }
  /* query the device capabilities */
  csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
  csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

  csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE,&acdb_buffer);
  if ( acdb_buffer == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_devices_of_feature: "
            "failed to allocate memory size of (%d)!",
                  CSD_ACDB_DEFAULT_BUF_SIZE);
    return CSD_ENORESOURCE;
  }

  acdb_cmd.nFeatureID = (uint32_t)feature_id;
  acdb_cmd.nBufferLength = CSD_ACDB_DEFAULT_BUF_SIZE;
  acdb_cmd.pBufferPointer = acdb_buffer;

  CSD_MSG( CSD_PRIO_MEDIUM,"csd_acdb_get_devices_of_feature: "
           "ACDB Query Devices of Feature" );

  acdb_rc = acdb_ioctl( acdb_command_id, (const byte*)&acdb_cmd,
                            sizeof(acdb_cmd), (uint8_t *)&acdb_cmd_response,
                            sizeof(acdb_cmd_response));

  if(ACDB_SUCCESS == acdb_rc)
  {
    if( *buf_size_used >= acdb_cmd_response.nBytesUsedInBuffer)
    {

      csd_memscpy(p_cap_dev_list, buf_size,
                  acdb_buffer, acdb_cmd_response.nBytesUsedInBuffer );
      *buf_size_used = acdb_cmd_response.nBytesUsedInBuffer;
      CSD_MSG( CSD_PRIO_MEDIUM,"csd_acdb_get_devices_of_feature: "
               "device capabilities got successfully");
      rc = CSD_EOK;
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_devices_of_feature: "
              " failed to copy memory size allocated (%d) needed (%d)",
              *buf_size_used, acdb_cmd_response.nBytesUsedInBuffer );
      rc = CSD_ENORESOURCE;
    }
  }
  else if( ACDB_DATA_NOT_FOUND == acdb_rc)
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_acdb_get_devices_of_feature: "
             "Devices of this Feature[%d] not found, "
             "Data Not Found(%d)", feature_id, acdb_rc );
    rc = csd_map_acdb_error_to_csd( acdb_rc );

  }
  else if(ACDB_SUCCESS != acdb_rc)
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_acdb_get_devices_of_feature: "
             "Failed to Retrieve Devices of this Feature[%d]"
             "May be Devices not Present(%d)", feature_id, acdb_rc );
    rc = csd_map_acdb_error_to_csd( acdb_rc );
  }
  csd_free(acdb_buffer);
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_is_dev_feature_enabled

  DESCRIPTION   Check if feature/capability is supported by device from ACDB

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    dev_id : TODO
                feature_id : TODO

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_is_dev_feature_enabled( int32_t dev_id, uint32_t feature_id)
{
  uint32_t i=0;
  int32_t rc = CSD_EOK;
  uint32_t* acdb_buffer = NULL;
  csd_acdb_dev_pair_list_type_t dev_pair_list = {0,};
  uint32_t actual_buf_size = CSD_ACDB_DEFAULT_BUF_SIZE;

  csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE,(void**)&acdb_buffer);
  if ( acdb_buffer == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_is_dev_feature_enabled: "
            "failed to allocate memory size of (%d)!",
            CSD_ACDB_DEFAULT_BUF_SIZE);
    return CSD_ENORESOURCE;
  }

  csd_memset(acdb_buffer, 0, CSD_ACDB_DEFAULT_BUF_SIZE);

  rc = csd_acdb_get_devices_of_feature(feature_id, (uint8_t *)acdb_buffer,
                                       CSD_ACDB_DEFAULT_BUF_SIZE,
                                       &actual_buf_size);
  if( CSD_EOK == rc )
  {
    dev_pair_list.noofdevs  = *(acdb_buffer);
    dev_pair_list.devList = (uint32_t*)(acdb_buffer + 1);

    rc = CSD_EFAILED;
    if(actual_buf_size == ( sizeof(uint32_t) +
                          ((sizeof(uint32_t)) * (dev_pair_list.noofdevs)) ) )
    {
      for(i=0; i<dev_pair_list.noofdevs; i++)
      {

        if(dev_id == dev_pair_list.devList[i])
        {
          rc = CSD_EOK;
          break;
        }
      }
    }
  }

  if(acdb_buffer != NULL)
  {
    csd_free(acdb_buffer);
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_afe_common_table_data

  DESCRIPTION   this function gets afe common calibration data as a blob

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    dev_id : TODO
                sample_rate : TODO
                afe_data : Buffer to where data needs to be populated
                buf_size : Allocated buffer size
                buf_size_used : Amount of buffer size used

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_afe_common_table_data(uint32_t dev_id,
                                           uint32_t sample_rate,
                                           uint8_t *afe_data,
                                           uint32_t buf_size,
                                           uint32_t *buf_size_used)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbAfeCommonTableCmdType acdb_cmd = {0,};
  AcdbQueryResponseType acdb_cmd_response = {0,};
  void *acdb_buffer = NULL;
  uint32_t acdb_command_id = ACDB_CMD_GET_AFE_COMMON_TABLE;

  /* Sanity check for Ptrs */
  if(NULL == afe_data || NULL == buf_size_used)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_afe_common_table_data: "
            "Null args: afe_data[0x%p], buf_size_used[0x%p]",
            (void*)afe_data, (void*)buf_size_used);
    return CSD_EBADPARAM;
  }
  *buf_size_used    = 0;

  /* Allocate for ACDB Buffer */
  csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE, &acdb_buffer );
  if ( acdb_buffer == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_afe_common_table_data: "
            "failed to allocate memory size of (%d)!",
            CSD_ACDB_DEFAULT_BUF_SIZE);
    return CSD_ENORESOURCE;
  }
  /* Init acdb command Response & i/o codec profile params */
  csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
  csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));
  
  /* Setup the ACDB Command */
  acdb_cmd.nDeviceId          = dev_id;
  acdb_cmd.nSampleRateId      = sample_rate;
  acdb_cmd.nBufferPointer     = acdb_buffer;
  acdb_cmd.nBufferLength      = CSD_ACDB_DEFAULT_BUF_SIZE;

  CSD_MSG(CSD_PRIO_LOW,"csd_acdb_get_afe_common_table_data: "
          "Querying AFE DATA for dev_id=[0x%x] sample_rate=[%d]",
          dev_id, sample_rate);

  /* query acdb */
  acdb_rc = acdb_ioctl(acdb_command_id, (const byte*)&acdb_cmd, sizeof(acdb_cmd),
                  (uint8_t *)&acdb_cmd_response, sizeof(acdb_cmd_response));
  if( ACDB_SUCCESS != acdb_rc )
  {
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_afe_common_table_data: "
            "Failed to retrieve AFE DATA from ACDB! Error(%d)",acdb_rc);
    rc = csd_map_acdb_error_to_csd( acdb_rc ); 
  }
  else
  {
    rc                = CSD_EOK;
    *buf_size_used    = acdb_cmd_response.nBytesUsedInBuffer;
    csd_memscpy(afe_data, buf_size,
                acdb_buffer, acdb_cmd_response.nBytesUsedInBuffer);

    CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_afe_common_table_data: "
            "AFE Data of Size=[0x%x] Retrieved Successfully!!",
            acdb_cmd_response.nBytesUsedInBuffer);
  }
  csd_free(acdb_buffer);

  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_dev_cap

  DESCRIPTION   Query ACDB for ACDB_CMD_GET_DEVICE_CAPABILITIES

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param capabilities      : Pointer where the resultant data would be
                                          stored
                @param entires_buf_size : Space allocated for entries in
                                          capabilities

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_acdb_get_dev_cap( csd_dev_caps_t *capabilites,
                                  uint32_t entries_buf_size )
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;

  AcdbDeviceCapabilitiesCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  void *acdb_buffer = NULL;

  /* query the device capabilities */
  csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
  csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

  if( NULL == capabilites )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_dev_cap: "
            "Null arg capabilites[0x%p]", (void*)capabilites);
    return CSD_EBADPARAM;
  }
  if( NULL == capabilites->entries )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_dev_cap: "
            "Null arg capabilites->entries[0x%p]",
            (void*)capabilites->entries);
    return CSD_EBADPARAM;
  }

  csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE, &acdb_buffer);
  if( acdb_buffer == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_dev_cap: "
            "failed to allocate memory size of (%d)!",
            CSD_ACDB_DEFAULT_BUF_SIZE);
    return CSD_ENORESOURCE;
  }
  if( CSD_EOK == rc )
  {
    acdb_cmd.nBufferLength = CSD_ACDB_DEFAULT_BUF_SIZE;
    acdb_cmd.nBufferPointer = acdb_buffer;

    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_acdb_get_dev_cap: "
             "Invoking ACDB_CMD_GET_DEVICE_CAPABILITIES");

    /* Avoiding Coupling rc's CSD_EOK with ACDB_SUCCESS */
    acdb_rc = acdb_ioctl(ACDB_CMD_GET_DEVICE_CAPABILITIES,
                         (const byte*) &acdb_cmd, sizeof(acdb_cmd),
                         (uint8_t *) &acdb_cmd_response,
                         sizeof(acdb_cmd_response));
    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG( CSD_PRIO_ERROR,"csd_dev_acdb_get_dev_cap: "
               "ACDB_CMD_GET_DEVICE_CAPABILITIES failed,"
               "acdb_rc[%d]", acdb_rc );
      rc = csd_map_acdb_error_to_csd(acdb_rc);
    }
    else
    {
      uint32_t acdb_entries_buf_size =
                        acdb_cmd_response.nBytesUsedInBuffer - sizeof(uint32_t);
      capabilites->num_devs = *(uint32_t *)acdb_buffer;
      if( entries_buf_size <= acdb_entries_buf_size )
      {
        csd_memscpy( capabilites->entries,entries_buf_size,
                     ((uint8_t *)acdb_buffer + sizeof(uint32_t)),
                     acdb_entries_buf_size );
        CSD_MSG( CSD_PRIO_MEDIUM, "ACDB_CMD_GET_DEVICE_CAPABILITIES success");
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "Insufficient size present in entries buffer"
                 ":need[%d], present[%d]",
                 acdb_entries_buf_size, entries_buf_size);
      }
    }
  }
  if( NULL != acdb_buffer )
  {
    /* Free allocated space */
    csd_free(acdb_buffer);
  }

  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_acdb_get_anc_devs

  DESCRIPTION   Query acdb for ACDB_CMD_GET_ANC_TX_DEVICE

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param rx_id : RX device ID
                @param p_tx_ids : Resultant TX device ID

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_acdb_get_anc_devs(uint32_t rx_id, uint32_t *p_tx_ids)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;

  AcdbAncDevicePairCmdType acdb_cmd;
  AcdbAncDevicePairRspType acdb_cmd_response;

  csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
  csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

  acdb_cmd.nRxDeviceId = rx_id;

  CSD_MSG(CSD_PRIO_MEDIUM, "Invoking ACDB_CMD_GET_ANC_TX_DEVICE,"
            "rx_dev_id[0x%x]", acdb_cmd.nRxDeviceId );

  /* query acdb */
  acdb_rc = acdb_ioctl(ACDB_CMD_GET_ANC_TX_DEVICE, (const byte*) &acdb_cmd,
                       sizeof(AcdbAncDevicePairCmdType),
                       (uint8_t *) &acdb_cmd_response,
                       sizeof(AcdbAncDevicePairRspType));
  if( ACDB_SUCCESS != acdb_rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "ACDB_CMD_GET_ANC_TX_DEVICE failed,"
            "acdb_rc[%d]", acdb_rc);
    rc = csd_map_acdb_error_to_csd(acdb_rc);
  }
  else
  {
    *p_tx_ids = acdb_cmd_response.nTxAncDeviceId;
    CSD_MSG(CSD_PRIO_MEDIUM, "ACDB_CMD_GET_ANC_TX_DEVICE success,"
            "ANC tx_dev_id[0x%x]", *p_tx_ids);
  }
  if( !*p_tx_ids )
  {
    rc = CSD_EFAILED;
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_adie_codec_profile

  DESCRIPTION   Query ACDB with codec path id to get codec related profile

  DEPENDENCIES  ACDB: AcdbAdiePathProfileV2CmdType,AcdbQueryResponseType
                WCD : Acdb_WCD93xx_B_CodecProfile (Specific to Taiko)

  PARAMETERS    @param codec_path_id : id specific to the related codec path
                        which needs to be queried
                @param codec_profile : reference to allocated space to copy
                        profiles got from acdb

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None

=============================================================================*/
#define CSD_ACDB_DEFAULT_CODEC_PATH_SIZE    4096
#define WCD_ACDB_COMMAND_MAX                ((CSD_ACDB_DEFAULT_CODEC_PATH_SIZE-\
                                              sizeof(adie_codec_acdb_header))\
                                             /sizeof(wcd_acdb_command))
typedef struct
{
  adie_codec_acdb_header header;
  wcd_acdb_command command[WCD_ACDB_COMMAND_MAX];
} Acdb_WCD93xx_B_CodecProfile;

int32_t csd_acdb_get_adie_codec_profile(uint32_t codec_path_id,
                                        uint32_t codec_param_id,
                                        uint8_t* _codec_profile)
{
  int32_t rc = CSD_EBADPARAM;
  int32_t acdb_rc = ACDB_SUCCESS;
  Acdb_WCD93xx_B_CodecProfile* codec_profile =
                                   (Acdb_WCD93xx_B_CodecProfile*)_codec_profile;
  /* Get From ACDB*/
  AcdbAdiePathProfileV2CmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  uint32_t buffer_size = 0;
  uint32_t expected_buffer_size = 0;
  uint32_t acdb_command_id = ACDB_CMD_GET_ADIE_CODEC_PATH_PROFILE_V2;

  /* Sanity check for Ptrs */
  if( NULL == codec_profile )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_adie_codec_profile: "
            "Failed to load codec profile:"
            "Function invoked with NULL parameter! rc=[0x%x]", rc);
    return rc;
  }

  /* Init acdb command Response & i/o codec profile params */
  csd_memset(&acdb_cmd, 0, sizeof(AcdbAdiePathProfileV2CmdType));
  csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));
  /*DALSYS_memset( codec_profile, 0, CSD_ACDB_DEFAULT_BUF_SIZE );*/

  /* Setup the ACDB Comamnd */
  acdb_cmd.ulCodecPathId = codec_path_id;
  acdb_cmd.nParamId = codec_param_id;
  acdb_cmd.nBufferPointer = (uint8_t*)codec_profile;
  acdb_cmd.nBufferLength = CSD_ACDB_DEFAULT_BUF_SIZE;

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_adie_codec_profile: "
          "Invoking ACDB_CMD_GET_ADIE_CODEC_PATH_PROFILE_V2,"
          "codecPathId[0x%x], codecParamId[0x%x]",
          acdb_cmd.ulCodecPathId, acdb_cmd.nParamId);

  /* query acdb */
  acdb_rc = acdb_ioctl(acdb_command_id, (const byte*) &acdb_cmd,
       sizeof(AcdbAdiePathProfileV2CmdType), (uint8_t *) &acdb_cmd_response,
       sizeof(AcdbQueryResponseType));
  if( ACDB_SUCCESS != acdb_rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_adie_codec_profile: "
            "ACDB_CMD_GET_ADIE_CODEC_PATH_PROFILE_V2 failed,"
            "acdb_rc[%d]",acdb_rc);
     rc = csd_map_acdb_error_to_csd(acdb_rc);
  }
  else
  {
    rc = CSD_EOK;
    buffer_size = acdb_cmd_response.nBytesUsedInBuffer;
    /*
     * Expected size is header size + N * command size
     * where N is num_action which is present in header
     */
    expected_buffer_size = sizeof(adie_codec_acdb_header)
        + codec_profile->header.num_actions*sizeof(wcd_acdb_command) ;

    /* Sanity check for the acdb data */
    if( buffer_size != expected_buffer_size)
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_adie_codec_profile: "
              "Codec path profile Data expected size is [%u],"
              "but got [%u]",expected_buffer_size,buffer_size);
      rc = CSD_EBADPARAM;
    }
    else
    {
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_adie_codec_profile: "
              "ACDB_CMD_GET_ADIE_CODEC_PATH_PROFILE_V2 success"
              ",retrieved codec path profile Data of Size[%d]", buffer_size);
    }
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_anc_profile

  DESCRIPTION   Query ACDB for ACDB_CMD_GET_CODEC_ANC_SETTING

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param a_dev : Reference to a device
                @param anc_profile : resultant anc profile
                @param buf_size : available buffer size

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_acdb_get_anc_profile( uint32_t dev_id,
                                      uint8_t *anc_profile,
                                      uint32_t buf_size)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbCodecANCSettingCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  void *acdb_buffer = NULL;
  uint32_t acdb_buf_size_used = 0;

  /* query the device info */
  csd_memset(&acdb_cmd, 0, sizeof(AcdbCodecANCSettingCmdType));
  csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

  csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE, &acdb_buffer);
  if( acdb_buffer == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_anc_profile: "
            "failed to allocate memory size of (%d)!",
        CSD_ACDB_DEFAULT_BUF_SIZE);
    return CSD_ENORESOURCE;
  }

  acdb_cmd.nRxDeviceId = dev_id;
  acdb_cmd.nParamId = ACDB_PID_CODEC_ANC_DATA_WCD9320;
  acdb_cmd.nBufferPointer = acdb_buffer;
  acdb_cmd.nBufferLength = CSD_ACDB_DEFAULT_BUF_SIZE;

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_anc_profile: "
          "Invoking ACDB_CMD_GET_CODEC_ANC_SETTING"
            "rx_dev_id[0x%x], ParamID[0x%x]", acdb_cmd.nRxDeviceId,
            acdb_cmd.nParamId );

  /* query acdb*/
  acdb_rc = acdb_ioctl(ACDB_CMD_GET_CODEC_ANC_SETTING, (const byte*) &acdb_cmd,
      sizeof(AcdbCodecANCSettingCmdType), (uint8_t *) &acdb_cmd_response,
      sizeof(AcdbQueryResponseType));
  if( ACDB_SUCCESS != acdb_rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_anc_profile: "
            "ACDB_CMD_GET_CODEC_ANC_SETTING failed,"
            "acdb_rc[%d]", acdb_rc);
    rc = csd_map_acdb_error_to_csd(acdb_rc);
  }
  else
  {
    acdb_buf_size_used = acdb_cmd_response.nBytesUsedInBuffer;
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_anc_profile: "
            "ACDB_CMD_GET_CODEC_ANC_SETTING success,"
            "retrieved codec ANC profile Data of Size[%d]",
            acdb_buf_size_used);
    csd_memscpy(anc_profile, buf_size, acdb_buffer, acdb_buf_size_used);
  }

  if( NULL != acdb_buffer )
  {
    /* Free allocated space */
    csd_free(acdb_buffer);
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_adie_codec_anc_db

  DESCRIPTION   Query ACDB settings for ANC block in the codec. 

  DEPENDENCIES  

  PARAMETERS    

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_adie_codec_anc_db(uint32_t rx_dev_id, 
                                       uint32_t codec_param_id, 
                                       uint8_t *anc_db_buf, 
                                       uint32_t *anc_buf_size)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  if( (NULL != anc_db_buf) && (NULL != anc_buf_size) )
  {
    AcdbCodecANCSettingCmdType acdb_cmd     = {0,};
    AcdbQueryResponseType acdb_cmd_response = {0,};

    csd_memset(&acdb_cmd, 0, sizeof(AcdbCodecANCSettingCmdType));
    csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

    acdb_cmd.nRxDeviceId    = rx_dev_id;
    /* codec_param_id for TAIKO => WCD9320 => ACDB_PID_CODEC_ANC_DATA_WCD9320 */
    acdb_cmd.nParamId       = codec_param_id;
    acdb_cmd.nBufferLength  = *anc_buf_size;
    acdb_cmd.nBufferPointer = anc_db_buf;

    CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_adie_codec_anc_db: "
            "Invoking ACDB_CMD_GET_CODEC_ANC_SETTING, "
            "rx_dev_id[0x%x], ParamID[0x%x]", acdb_cmd.nRxDeviceId,
            acdb_cmd.nParamId );

    acdb_rc = acdb_ioctl(ACDB_CMD_GET_CODEC_ANC_SETTING, (const byte*) &acdb_cmd,
                    sizeof(acdb_cmd), (uint8_t *) &acdb_cmd_response,
                    sizeof(acdb_cmd_response));
    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_adie_codec_anc_db: "
              "ACDB_CMD_GET_CODEC_ANC_SETTING failed, "
              "acdb_rc[%d], nBytesUsedInBuffer[%d]",acdb_rc,
              acdb_cmd_response.nBytesUsedInBuffer);
      rc = csd_map_acdb_error_to_csd(acdb_rc);
    }
    else
    {
      rc = CSD_EOK;
      *anc_buf_size = acdb_cmd_response.nBytesUsedInBuffer;
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_adie_codec_anc_db: "
              "ACDB_CMD_GET_CODEC_ANC_SETTING success, "
              "got the ADIE ANC data of size[%d]!",
              acdb_cmd_response.nBytesUsedInBuffer);
    }
}
else
{
  CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_adie_codec_anc_db: "
          "Null parameter passed, anc_db_buf[%p], "
          "anc_buf_size[%p]", (void*)anc_db_buf, (void*)anc_buf_size);
  rc = CSD_EFAILED;
}
return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_adie_sidetone

  DESCRIPTION   Query acdb for ACDB_CMD_GET_ADIE_SIDETONE_TABLE

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param rx_id : Rx device id
                @param tx_id : Tx device id
                @param adie_sidetone_tbl : Resultant sidetone table
                @param buf_size : Buffer size


  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_adie_sidetone(uint32_t rx_id, uint32_t tx_id,
                                   uint8_t *adie_sidetone_tbl,
                                   uint32_t buf_size)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbAdieSidetoneTableCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  csd_acdb_adie_sidetone_info_t acdb_buffer;
  uint32_t acdb_command_id = ACDB_CMD_GET_ADIE_SIDETONE_TABLE;

  /* query the device info */
  csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
  csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));
  csd_memset(&acdb_buffer, 0, sizeof(acdb_buffer));  

  acdb_cmd.nRxDeviceId = rx_id;
  acdb_cmd.nTxDeviceId = tx_id;
  acdb_cmd.nBufferPointer = (uint8_t *)&acdb_buffer; //sidetone_info_buf;
  acdb_cmd.nBufferLength = sizeof(acdb_buffer);

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_adie_sidetone: "
          "Invoking ACDB_CMD_GET_ADIE_SIDETONE_TABLE,"
              "rx_dev_id[0x%x], tx_dev_id[0x%x]", acdb_cmd.nRxDeviceId,
              acdb_cmd.nTxDeviceId );

  /* query acdb */
  if (ACDB_SUCCESS != (acdb_rc = acdb_ioctl(acdb_command_id,
                                       (const byte *)&acdb_cmd,
                                       sizeof(acdb_cmd),
                                       (uint8_t *)&acdb_cmd_response,
                                       sizeof(acdb_cmd_response))))
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_adie_sidetone: "
            "ACDB_CMD_GET_ADIE_SIDETONE_TABLE failed,"
            " acdb_rc[%d]", acdb_rc);
    rc = csd_map_acdb_error_to_csd(acdb_rc);
  } 
  else
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_adie_sidetone: "
            "ACDB_CMD_GET_ADIE_SIDETONE_TABLE success,"
            "retrieved ADIE sidetone table of size[%d]!",
            acdb_cmd_response.nBytesUsedInBuffer);
    csd_memscpy(adie_sidetone_tbl, buf_size,
                acdb_buffer.param_data, sizeof(acdb_buffer.param_data));
  }

  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_afe_sidetone

  DESCRIPTION   Query acdb for ACDB_CMD_GET_AFE_DATA

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param rx_id : Rx device id
                @param tx_id : Tx device id
                @param gain : Resultant sidetone gain, uint16[2]

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_acdb_get_afe_sidetone(uint32_t rx_id, uint32_t tx_id,
                                      bool_t *enable, uint16_t *gain)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;

  AcdbAfeDataCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  uint16_t acdb_buffer[2];

  /* query the device info */
  csd_memset(&acdb_cmd, 0, sizeof(AcdbAfeDataCmdType));
  csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));


  acdb_cmd.nRxDeviceId = rx_id;
  acdb_cmd.nTxDeviceId = tx_id;
  acdb_cmd.nModuleId = ACDB_MID_SIDETONE;
  acdb_cmd.nParamId = ACDB_PID_SIDETONE;
  acdb_cmd.nBufferPointer = (uint8_t*)acdb_buffer;
  acdb_cmd.nBufferLength = 2*sizeof(uint16_t);

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_afe_sidetone: "
          "Invoking ACDB_CMD_GET_AFE_DATA,"
          "rx_dev_id[0x%x], tx_dev_id[0x%x], ModuleID[0x%x],"
          "ParamID[0x%x]", acdb_cmd.nRxDeviceId, acdb_cmd.nTxDeviceId,
          acdb_cmd.nModuleId, acdb_cmd.nParamId );

  /* query acdb */
  acdb_rc = acdb_ioctl(ACDB_CMD_GET_AFE_DATA, (const byte*) &acdb_cmd,
      sizeof(AcdbAfeDataCmdType), (uint8_t *) &acdb_cmd_response,
      sizeof(AcdbQueryResponseType));

  /* gain is an array with 2 element, each of size uint16(2bytes) */
  if( (ACDB_SUCCESS != acdb_rc) ||
      (acdb_cmd_response.nBytesUsedInBuffer != (2*sizeof(uint16_t))) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_acdb_get_afe_sidetone: "
            "ACDB_CMD_GET_AFE_DATA failed, acdb_rc[%d]",acdb_rc);
    rc = csd_map_acdb_error_to_csd(acdb_rc);
  }
  else
  {
    /* acdb_buffer contains 2 uint16_t values - enable flag and gain */
    *enable = (bool_t)acdb_buffer[0];
    *gain   = acdb_buffer[1];
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_afe_sidetone: "
            "ACDB_CMD_GET_AFE_DATA success,"
            " retrieved sidetone gain[0x%x]", *gain );
  }

  return rc;
}


/*=============================================================================

  FUNCTION      csd_acdb_get_afe_sidetone_iir_enable

  DESCRIPTION   Query acdb to get the iir filter enable info

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param rx_id : Rx device id
                @param tx_id : Tx device id
                @param afe_mod_enable_param_t *iir_enable

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_acdb_get_afe_sidetone_iir_enable(uint32_t rx_id, uint32_t tx_id,
                                             bool_t *iir_enable)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbAfeDataCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;
  afe_mod_enable_param_t afe_mod_flag;

  /* query the device info */
  csd_memset(&acdb_cmd, 0, sizeof(AcdbAfeDataCmdType));
  csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

  acdb_cmd.nRxDeviceId = rx_id;
  acdb_cmd.nTxDeviceId = tx_id;
  acdb_cmd.nModuleId = AFE_MODULE_SIDETONE_IIR_FILTER;
  acdb_cmd.nParamId = AFE_PARAM_ID_ENABLE;
  acdb_cmd.nBufferPointer = (uint8_t *)&afe_mod_flag;
  acdb_cmd.nBufferLength = sizeof(afe_mod_enable_param_t);

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_afe_sidetone_iir_enable: "
          "Invoking ACDB_CMD_GET_AFE_DATA, "
          "rx_dev_id[0x%x], tx_dev_id[0x%x], ModuleID[0x%x], "
          "ParamID[0x%x]", acdb_cmd.nRxDeviceId, acdb_cmd.nTxDeviceId,
          acdb_cmd.nModuleId, acdb_cmd.nParamId );

  /* query acdb */
  acdb_rc = acdb_ioctl(ACDB_CMD_GET_AFE_DATA, (const byte*) &acdb_cmd,
       sizeof(AcdbAfeDataCmdType), (uint8_t *) &acdb_cmd_response,
       sizeof(AcdbQueryResponseType));

  /* enable is an array with 2 element, each of size uint16(2bytes) */
  if( (ACDB_SUCCESS != acdb_rc) ||
      (acdb_cmd_response.nBytesUsedInBuffer != (sizeof(afe_mod_enable_param_t))) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_acdb_get_afe_sidetone_iir_enable: "
            "ACDB_CMD_GET_AFE_DATA failed, "
            "acdb_rc[%d], nBytesUsedInBuffer[%d]",
            acdb_rc, acdb_cmd_response.nBytesUsedInBuffer);
    rc = csd_map_acdb_error_to_csd(acdb_rc);
  }
  else
  {
    *iir_enable = afe_mod_flag.enable;
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_afe_sidetone_iir_enable: "
            "ACDB_CMD_GET_AFE_DATA success, "
            "retrieved sidetone iir enable[0x%x]", *iir_enable );
  }

  return rc;
}

/*==============================================================================

  FUNCTION      csd_acdb_get_afe_sidetone_iir_filter_data

  DESCRIPTION   Query acdb to get the iir filter data  info

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param rx_id : Rx device id
                @param tx_id : Tx device id
                @param iir_data : Resultant sidetone IIR data

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_acdb_get_afe_sidetone_iir_data(uint32_t rx_id, uint32_t tx_id,
                                               void* iir_data, uint32_t buf_len)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbAfeDataCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;

  if( NULL != iir_data)
  {
    /* query the device info */
    csd_memset(&acdb_cmd, 0, sizeof(AcdbAfeDataCmdType));
    csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

    acdb_cmd.nRxDeviceId = rx_id;
    acdb_cmd.nTxDeviceId = tx_id;
    acdb_cmd.nModuleId = AFE_MODULE_SIDETONE_IIR_FILTER;
    acdb_cmd.nParamId = AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG;
    acdb_cmd.nBufferPointer = iir_data;
    acdb_cmd.nBufferLength = buf_len;

    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_acdb_get_afe_sidetone_iir_data: "
            "Invoking ACDB_CMD_GET_AFE_DATA, "
            "rx_dev_id[0x%x], tx_dev_id[0x%x], ModuleID[0x%x], "
            "ParamID[0x%x]", acdb_cmd.nRxDeviceId, acdb_cmd.nTxDeviceId,
            acdb_cmd.nModuleId, acdb_cmd.nParamId );

    /* query acdb */
    acdb_rc = acdb_ioctl(ACDB_CMD_GET_AFE_DATA, (const byte*) &acdb_cmd,
        sizeof(AcdbAfeDataCmdType), (uint8_t *) &acdb_cmd_response,
        sizeof(AcdbQueryResponseType));

    /* check the ACDB status */
    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_acdb_get_afe_sidetone_iir_data: "
              "ACDB_CMD_GET_AFE_DATA failed, "
              "acdb_rc[%d]", acdb_rc);
      rc = csd_map_acdb_error_to_csd(acdb_rc);
    }
    else
    {
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_acdb_get_afe_sidetone_iir_data: "
              "ACDB_CMD_GET_AFE_DATA success "
              "Side tone IIR data size[%d]",
               acdb_cmd_response.nBytesUsedInBuffer);
      if( acdb_cmd_response.nBytesUsedInBuffer > buf_len )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_afe_sidetone_iir_data: "
                "Failed to copy the IIR data, ACDB return "
                "more data [%d] than IIR filter"
                "structure size ", acdb_cmd_response.nBytesUsedInBuffer);
        rc = CSD_ENORESOURCE;
      }
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_acdb_get_afe_sidetone_iir_data: "
            "Invalid input param:"
    "Function invoked with NULL parameter! rc[0x%x]", rc);
  }
  return rc;
}
/*==============================================================================

  FUNCTION      csd_acdb_get_afe_common_data

  DESCRIPTION   Common interface for afe module to query data from ACDB

  DEPENDENCIES  ACDB related structures
                @see acdb.h

  PARAMETERS    @param dev_id : a device id
                @param sample_rate : dev sample_rate
                @param afe_module_id : afe module id
                @param afe_param_id : afe param id
                @param p_buffer : reference to buffer where data would be filled
                @param buffer_length : space available in (p_buffer)
                @param bytes_filled : 0 or buffer_length depending on whether
                                      ACDB query was successful or not.

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_acdb_get_afe_common_data(uint32_t dev_id,
                                     uint32_t sample_rate,
                                     uint32_t afe_module_id,
                                     uint32_t afe_param_id,
                                     uint8_t *p_buffer,
                                     uint32_t buffer_length,
                                     uint32_t *bytes_filled)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  uint32_t acdb_command_id = ACDB_CMD_GET_AFE_COMMON_DATA;
  uint8_t * acdb_buffer = NULL;
  AcdbAfeCmnDataCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;

  if( NULL == p_buffer || NULL == bytes_filled )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_afe_common_data: "
             "NULL Param for Dev_Id=[%d], p_buffer=[%p], "
             "bytes_filled=[%p]", dev_id, (void*)p_buffer, (void*)bytes_filled);
    rc = CSD_EBADPARAM;
    return rc;
  }

  *bytes_filled = 0;

  /*Allocate Memory*/
  csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE,(void**)&acdb_buffer);
  if ( acdb_buffer == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_acdb_get_afe_common_data: "
            "Failed to allocate memory size of (%d)!",
            CSD_ACDB_DEFAULT_BUF_SIZE);
    rc = CSD_ENORESOURCE;
  }
  else
  {
    /* Initialize acdb cmd and response Data */
    csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
    csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

    /* Setup the ACDB Command */
    acdb_cmd.nDeviceId        = dev_id;
    /* TODO: Unni Can we use the argument buffer_length and avoid malloc and memcpy */
    acdb_cmd.nBufferLength    = CSD_ACDB_DEFAULT_BUF_SIZE;
    acdb_cmd.nBufferPointer   = (void*)acdb_buffer;
    acdb_cmd.nAfeSampleRateId = sample_rate; 
    acdb_cmd.nModuleId        = afe_module_id; /* AFE_MODULE_SPEAKER_PROTECTION; */
    acdb_cmd.nParamId         = afe_param_id; /* AFE_PARAM_ID_ENABLE; AFE_PARAM_ID_SPKR_PROT_CONFIG */

    CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_afe_common_data: "
            "Invoking ACDB_CMD_GET_AFE_COMMON_DATA,"
            "DeviceID[0x%x], AfeSampleRateID[0x%x], ModuleID[0x%x],"
            "ParamID[0x%x]", acdb_cmd.nDeviceId, acdb_cmd.nAfeSampleRateId,
            acdb_cmd.nModuleId, acdb_cmd.nParamId );

    /* query acdb */
    acdb_rc = acdb_ioctl(acdb_command_id,(const byte*)&acdb_cmd,
                    sizeof(acdb_cmd),(uint8_t *)&acdb_cmd_response,
                    sizeof(acdb_cmd_response));
    if( ACDB_SUCCESS != acdb_rc)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_afe_common_data: "
              "ACDB_CMD_GET_AFE_COMMON_DATA failed, acdb_rc[%d]", acdb_rc);
      *bytes_filled = 0;
      /* Careful: re-mapping acdb error code to csd error code
       * TODO: Unni rc is having dual functionality, maintainence vice better
       * to have separate rc variables */
      rc = csd_map_acdb_error_to_csd(acdb_rc);
    }
    else
    {
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_afe_common_data: "
              "ACDB_CMD_GET_AFE_COMMON_DATA success,"
              "information returned with Bytes[%d]",
              acdb_cmd_response.nBytesUsedInBuffer);
      if(acdb_cmd_response.nBytesUsedInBuffer == buffer_length )
      {
        *bytes_filled = acdb_cmd_response.nBytesUsedInBuffer;
        csd_memscpy(p_buffer, buffer_length,
                    acdb_buffer, acdb_cmd_response.nBytesUsedInBuffer);
        rc = CSD_EOK;
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_afe_common_data: "
                 "ACDB_CMD_GET_AFE_COMMON_DATA Bytes Allocated "
                 "for the source  buffer is wrong[%d]!", buffer_length);
        *bytes_filled = 0;
        rc = CSD_ENEEDMORE;
      }
    }

    /* Free the Memory */
    csd_free(acdb_buffer);
  }

  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_audio_record_device_pair

  DESCRIPTION   Query acdb to get the paried Rx device for audio record with EC

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 
                @see csd_dev_device_info.h

  PARAMETERS    @param tx_id  : Tx device id {Input/Key]
                @param *rx_id : Rx device id [Output]

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_audio_record_device_pair(uint32_t tx_dev_id, uint32_t *rx_dev_id)
{
   int32_t rc = CSD_EOK;
   int32_t acdb_rc = ACDB_SUCCESS;
  /* Sanity check on the input params */
  if((NULL != rx_dev_id))
  {
    *rx_dev_id = CSD_DEV_ID_INVALID;
    if((CSD_DEV_ID_INVALID != tx_dev_id))
    {
      AcdbDevicePairListCmdType  acdb_cmd = {0};
      AcdbQueryResponseType  acdb_cmd_response = {0,};
      uint32_t *acdb_buffer = NULL;
      uint32_t acdb_command_id = ACDB_CMD_GET_DEVICE_PAIR_LIST;
              
      csd_malloc(CSD_ACDB_DEFAULT_BUF_SIZE,(void**)&acdb_buffer);
      if (NULL == acdb_buffer )
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_audio_record_device_pair: "
                "Failed to allocate memory size of (%d)!",
                CSD_ACDB_DEFAULT_BUF_SIZE);
        return CSD_ENORESOURCE; 
      }
      
      csd_memset(acdb_buffer, 0, CSD_ACDB_DEFAULT_BUF_SIZE);
      
      /* Initialize acdb cmd and response Data */
      csd_memset(&acdb_cmd, 0, sizeof(acdb_cmd));
      csd_memset(&acdb_cmd_response, 0, sizeof(acdb_cmd_response));

      /* Setup the ACDB Command */
      acdb_cmd.nDevPairType   = ACDB_AUDIO_RECORD_TX2RX_LIST;
      acdb_cmd.nDeviceID      = tx_dev_id;
      acdb_cmd.nBufferLength  = CSD_ACDB_DEFAULT_BUF_SIZE;
      acdb_cmd.pBufferPointer = (void*)acdb_buffer;
  
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_audio_record_device_pair: "
              "Invoking ACDB_CMD_GET_DEVICE_PAIR_LIST,"
              "tx_dev_id[0x%x], DevPairType[0x%x]",
              acdb_cmd.nDeviceID, acdb_cmd.nDevPairType );

      /* query acdb */
      acdb_rc = acdb_ioctl(acdb_command_id,(const byte*)&acdb_cmd, 
              sizeof(acdb_cmd),(uint8_t *)&acdb_cmd_response, 
              sizeof(acdb_cmd_response));
      if( (ACDB_SUCCESS != acdb_rc) ||
          (acdb_cmd_response.nBytesUsedInBuffer < (sizeof(csd_acdb_dev_pair_list_type_t))) )
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_audio_record_device_pair: "
                 "ACDB_CMD_GET_DEVICE_PAIR_LIST failed,"
                 "acdb_rc[%d], RespBufLen[%d]", acdb_rc,
                 acdb_cmd_response.nBytesUsedInBuffer );
        rc = csd_map_acdb_error_to_csd(acdb_rc); 
      }
      else
      {
        uint32_t nDevices  = *(acdb_buffer);
        uint32_t *pDevList = ((acdb_buffer + 1));
        if( 0 == nDevices)
        {
          rc = CSD_EFAILED;
          CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_audio_record_device_pair: "
                   "Error!! Empty List in the RespBufLen[%d]!", 
               nDevices );
        }
        else if(1 == nDevices)
        {
          *rx_dev_id = pDevList[0];
          rc = CSD_EOK;
          CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_audio_record_device_pair: "
                  "ACDB_CMD_GET_DEVICE_PAIR_LIST success,"
                  " tx_dev_id[0x%x] is paired with rx_dev_id[0x%x]",
                  tx_dev_id, *rx_dev_id);
        }
        else
        {
          rc = CSD_EFAILED;
          CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_get_audio_record_device_pair: "
                   "Error!! tx_dev_id[0x%x] is paired with More"
                   " than one, \"[%d]\" Rx Device!!!",
                   tx_dev_id, nDevices );
        }
      }
      if( NULL != acdb_buffer )
      {
        /* Free allocated space */
        csd_free(acdb_buffer);
      }
    }
    else
    {
      CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_get_audio_record_device_pair: "
               "Invalid LocalDeviceID!!"
               "Failed to retrieve mapping ID for tx_dev_id[0x%x]",
               tx_dev_id);
      rc = CSD_EBADPARAM;
    }
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR,"csd_acdb_get_audio_record_device_pair: "
             "NULL Remote Mapped Rx Device!!"
             " for tx_dev_id[0x%x]", tx_dev_id);
    rc = CSD_EBADPARAM;
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_dev_prop_size

  DESCRIPTION   Get device property size info for given device ID

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 

  PARAMETERS    @param uint32_t device_id
                @param uint32_t dev_prop_id
                @param uint32_t* dev_size

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_dev_prop_size(uint32_t device_id,uint32_t dev_prop_id,
                                   uint32_t* dev_size)
{
  int32_t rc = CSD_EOK;
  int32_t  acdb_rc = ACDB_SUCCESS;
  AcdbDevPropSizeCmdType acdb_cmd;
  AcdbSizeResponseType acdb_size_response;

  if(NULL != dev_size)
  {
    /* reset the acdb cmd buffers */
    csd_memset(&acdb_cmd, 0, sizeof(AcdbDevPropSizeCmdType));
    csd_memset(&acdb_size_response, 0, sizeof(AcdbSizeResponseType));

    acdb_cmd.nDeviceId = device_id;
    acdb_cmd.nPropID   = dev_prop_id;

    CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_dev_prop_size: "
            "Invoking ACDB_CMD_GET_DEVICE_PROPERTY_SIZE,"
            "DeviceID[0x%x], PropID[0x%x]", acdb_cmd.nDeviceId, acdb_cmd.nPropID );

    /* query acdb */
    acdb_rc = acdb_ioctl(ACDB_CMD_GET_DEVICE_PROPERTY_SIZE, (const byte*) &acdb_cmd,
                   sizeof(AcdbDevPropSizeCmdType), (uint8_t *) &acdb_size_response,
                   sizeof(AcdbSizeResponseType));

    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_prop_size: "
              "ACDB_CMD_GET_DEVICE_PROPERTY_SIZE failed,"
                       " acdb_rc[%d]", acdb_rc);
      rc = csd_map_acdb_error_to_csd ( acdb_rc );
    }
    else
    {
      /*update the size*/
      *dev_size = acdb_size_response.nSize;
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_acdb_get_dev_prop_size: "
              "ACDB_CMD_GET_DEVICE_PROPERTY_SIZE success,"
                      " device property size[0x%x]",*dev_size);
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_prop_size: "
            "Function invoked with NULL parameter, "
                      "dev_size[%p], rc[0x%x]", (void*)dev_size, rc);
  }
  return rc;
}


/*=============================================================================

  FUNCTION      csd_acdb_get_dev_prop_info

  DESCRIPTION   Get device property info for given device ID

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 

  PARAMETERS    @param uint32_t device_id
                @param uint32_t dev_prop
                @param uint32_t buffer_length
                @param uint32_t *dev_prop_data

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_acdb_get_dev_prop_info(uint32_t device_id,uint32_t dev_prop_id,
                            uint32_t buffer_length,uint32_t *dev_prop_data)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbDevPropCmdType acdb_cmd;
  AcdbQueryResponseType acdb_cmd_response;

  if(NULL != dev_prop_data)
  {
    /* reset the acdb cmd buffers */
    csd_memset(&acdb_cmd, 0, sizeof(AcdbDevPropCmdType));
    csd_memset(&acdb_cmd_response, 0, sizeof(AcdbQueryResponseType));

    acdb_cmd.nDeviceId      = device_id;
    acdb_cmd.nPropID        = dev_prop_id;
    acdb_cmd.pBufferPointer = (uint8_t *)dev_prop_data;
    acdb_cmd.nBufferLength  = buffer_length; 

    CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_dev_prop_info: "
            "Invoking ACDB_CMD_GET_DEVICE_PROPERTY,"
            "DeviceID[0x%x], PropID[0x%x]", acdb_cmd.nDeviceId, acdb_cmd.nPropID);

    /* query acdb */
    acdb_rc = acdb_ioctl(ACDB_CMD_GET_DEVICE_PROPERTY, (const byte*) &acdb_cmd,
                   sizeof(AcdbDevPropCmdType), (uint8_t *) &acdb_cmd_response,
                   sizeof(AcdbQueryResponseType));

    if( ACDB_SUCCESS != acdb_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_prop_info: "
              "ACDB_CMD_GET_DEVICE_PROPERTY failed,"
              " acdb_rc[%d]",acdb_rc);
      rc = csd_map_acdb_error_to_csd ( acdb_rc );
    }
    else
    {
      CSD_MSG(CSD_PRIO_HIGH, "csd_acdb_get_dev_prop_info: "
              "ACDB_CMD_GET_DEVICE_PROPERTY success");
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_dev_prop_info: "
            "Function invoked with NULL parameter,"
            "dev_prop_data[%p], rc[0x%x]", (void*)dev_prop_data, rc);
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_acdb_get_device_avsync_hw_delay

  DESCRIPTION   Query ACDB for Avsync HW delay for a given device ID and sample rate

  DEPENDENCIES  ACDB related structures 
                @see acdb.h 

  PARAMETERS    @param uint32_t device_id
                @param uint32_t sample_rate
                @param uint32_t *hw_delay

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/

int32_t csd_acdb_get_device_avsync_hw_delay(uint32_t device_id,uint32_t sample_rate,
                                            uint32_t* hw_delay)
{
  int32_t rc = CSD_EOK;
  uint32_t* av_sync_buf = NULL;
  uint32_t  i = 0, hw_avsync_buf_size = 0, no_entries = 0;
  csd_acdb_avsync_hw_delay_info_t* av_sync = NULL;

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_acdb_get_device_avsync_hw_delay: "
          "Querying AVSync info");
 
  if( hw_delay == NULL )
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_device_avsync_hw_delay: "
            "Invalid input parameter, hw_delay[%p],"
                           " rc[0x%x]", (void*)hw_delay, rc);
  }
  if( CSD_EOK == rc)
  {
    /* query avsync property size info from acdb*/
    rc = csd_acdb_get_dev_prop_size(device_id,ACDB_AVSYNC_INFO,
                             &hw_avsync_buf_size);
    if( rc != CSD_EOK )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_device_avsync_hw_delay: "
              "ACDB_AVSYNC_INFO, get property size:"
                    " error, rc[0x%x] size[%d] ",rc,hw_avsync_buf_size);
    }
    if( CSD_EOK == rc )
    {
      csd_malloc(hw_avsync_buf_size,(void**)&av_sync_buf);
      if ( av_sync_buf == NULL )
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_device_avsync_hw_delay: "
                "Failed to allocate memory size of [%d]!",
                         hw_avsync_buf_size);
        rc = CSD_ENORESOURCE;
      }
    }
  }

  if( CSD_EOK == rc)
  {
    /* query avsync info from acdb*/
    rc = csd_acdb_get_dev_prop_info(device_id,ACDB_AVSYNC_INFO,
                             hw_avsync_buf_size,av_sync_buf);
    if( rc != CSD_EOK )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_device_avsync_hw_delay: "
              "ACDB_AVSYNC_INFO, get device property:"
                             " error, rc[0x%x]",rc);
    }
  }

  if( CSD_EOK == rc )
  {
    no_entries = *(av_sync_buf);
    av_sync = (csd_acdb_avsync_hw_delay_info_t* ) (av_sync_buf + 1);

    /* obtain the hw delay value for given sample rate from the info returned from acdb */
    if( no_entries == (((hw_avsync_buf_size - (sizeof(no_entries)))/(sizeof(csd_acdb_avsync_hw_delay_info_t)))))
    {
      for(i = 0;i < no_entries; i++)
      {
        if( av_sync->sample_rate == sample_rate )
        {
          *hw_delay = av_sync->delay_us;
          CSD_MSG(CSD_PRIO_HIGH,"csd_acdb_get_device_avsync_hw_delay: "
                  "AV Sync HW Delay: [%d] for "
                    "Sample rate:[%d]",*hw_delay,av_sync->sample_rate);
          break;
        }
        ++av_sync;
      }
      if( i == no_entries )
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_device_avsync_hw_delay: "
                "AV Sync HW Delay: valid Entry not found "
                         "for given sample rate: (%d)",sample_rate);
        rc = CSD_ENOTFOUND;
      }
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_acdb_get_device_avsync_hw_delay: "
              "AV Sync HW Delay: Un supported no of entries:(%d)"
              "max buffer size : [0x%x]",no_entries,hw_avsync_buf_size );
      rc = CSD_EUNSUPPORTED;
    }
  }

  if( av_sync_buf != NULL )
  {
    csd_free(av_sync_buf);
  }
  return rc;
}
