/*==============================================================================
  FILE:         csd_dev_afe.c

  OVERVIEW:     The objective of this module is to provide an interface for
                device modules to interface with LPASS AFE module for requested
                commands.

  Dependencies: None

  Copyright (c) 2008-13, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/b-family/src/csd_dev_afe.c#2 $
  $DateTime: 2019/01/03 07:23:13 $
  $Author: pwbldsvc $
  $Change: 17964421 $
  $Revision: #2 $

  when       who    what, where, why
  --------   ---    ----------------------------------------------------------
  01/24/18   pm     remove clk_id assignment in csd dev afe layer     
  11/15/16   rk     Abstracting OSAL layer
  02/12/14   jk     Added support for CSD_DEV_CMD_RESTART   
  11/09/13   unni   afe side tone fix
  10/07/13   vk     added support for AVSync feature
  07/01/13   rp     Added RT proxy device support
  06/05/13   unni   afe sanity checks
  03/04/13   suns   Device Module Unification
  01/10/13   rp     Added SideTone IIR filter support
  07/13/12   rp     Added 9x25 AFE support.
  01/17/12   jp     Support for DTMF thru AFE
  27/04/10   suns   Initial Creation
==============================================================================*/
#include "csd_dev_afe.h"
#include "csd_q6atl.h"
#include "csd_q6msg.h"
#include "csd_q6msg_def.h"
#include "DALSys.h"
#include "csd_debug_msg.h"
#include "csd_dev_main.h"
#include "csd_handle.h"
#include "csd_dev_acdb.h"
#include "csd_dev_device_info.h"
#include "csd_dev_acdb_private.h"   /* Queries to ACDB to retrieve AFE params */
#include "csd_dev_codec_private.h"  /* For AANC CFG Data Retrieval from codec */
#include "csd_dev_speaker_protection.h"
#include "csd_os_dependencies.h"

#define CSD_DEV_AFE_PORT_NOT_CONFIGURED (0)
#define CSD_DEV_AFE_PORT_CONFIGURED     (1)
#define CSD_DEV_AFE_PORT_ACTIVE         (2)

#define CSD_DEV_PORT_INDICES_MAX (AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE +\
                                  AFE_PORT_ID_SLIMBUS_RANGE_SIZE       +\
                                  AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE +\
                                  AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE)
#define BETWEEN(a, b, c)  (((a) >= (b)) && ((a) <= (c)))

typedef struct
{
  uint8_t port_status; /* CSD_AFE_PORT_CONFIGURED/NOT_CONFIGURED */
  uint16_t ref_count;  /* Reference Counter for the activation state */
}csd_dev_afe_port_info_t;

static csd_dev_afe_port_info_t csd_dev_afe_port_info[CSD_DEV_PORT_INDICES_MAX];

/******************************************************************************
 * Internal functions                                                         *
 ******************************************************************************/
/*=============================================================================

  FUNCTION      csd_dev_get_port_index

  DESCRIPTION   get index of csd_dev_afe_port_info for the afe port type

  DEPENDENCIES  

  PARAMETERS    @param port : port id

  RETURN VALUE  valid index on success, invalid on failure
  SIDE EFFECTS  none

=============================================================================*/
static uint32_t csd_dev_get_port_index(uint16_t port)
{
  int32_t index = CSD_DEV_PORT_INDICES_MAX;
  if( BETWEEN(port, AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START,
                    AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END) )
  {
    /* I2S/PCM/DIGITAL_MIC/HDMI ports */
    index = port - AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START;
  }
  else if(BETWEEN(port, AFE_PORT_ID_SLIMBUS_RANGE_START,
                        AFE_PORT_ID_SLIMBUS_RANGE_END) )
  {
    /* Slimbus */
    index = port - AFE_PORT_ID_SLIMBUS_RANGE_START;
    /* Adding offset for IF ports */
    index += AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE;

  }
  else if(BETWEEN(port, AFE_PORT_ID_RT_PROXY_PORT_RANGE_START,
                        AFE_PORT_ID_RT_PROXY_PORT_RANGE_END))
  {
    /* RT Proxy ports */
   index = port - AFE_PORT_ID_RT_PROXY_PORT_RANGE_START;
   /* Adding offset for IF and Slimbus ports */
   index += AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE +
            AFE_PORT_ID_SLIMBUS_RANGE_SIZE;
  }
  else if(BETWEEN(port, AFE_PORT_ID_PSEUDOPORT_RANGE_START,
                        AFE_PORT_ID_PSEUDOPORT_RANGE_END) )
  {
    /* Pseudo ports */
   index = port - AFE_PORT_ID_PSEUDOPORT_RANGE_START;
   /* Adding offset for IF , Slimbus and RT proxy ports */
   index += AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE +
            AFE_PORT_ID_SLIMBUS_RANGE_SIZE+
            AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE;
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_get_port_index: Unknown port %d",port);
    index = CSD_DEV_PORT_INDICES_MAX;
  }
  return index;
}


static void get_common_q6_afe_msg_hdr( struct aprv2_packet_t *hdr, uint32_t cmd )
{
  /* set up q6 MSG header */
  Q6MSG_SET_HDR_VER(hdr->header, Q6MSG_VERSION);
  Q6MSG_SET_HDR_SIZE(hdr->header, Q6MSG_PACKED_HEADER_SIZE);
  Q6MSG_SET_MSG_TYPE(hdr->header, Q6MSG_TYPE_SQCMD);
  Q6MSG_SET_PACKET_SIZE(hdr->header, Q6MSG_PACKED_HEADER_SIZE);
  hdr->src_addr |= AUDIO_DOMIAN << 8;
  hdr->src_addr |= CSD_SERVICE_DM;
  hdr->src_port |= CSD_DEV_AFE_SRC_PORT_HIGH;
  hdr->src_port |= CSD_DEV_AFE_SRC_PORT_LOW;
  hdr->token = csd_q6atl_gen_apr_token();
  hdr->dst_addr |= ADSP_DOMAIN << 8;
  hdr->dst_addr |= ADSP_AFE_SERVICE;
  hdr->opcode = cmd;
  return;
}

/* Function to retrieve the proper channel allocation based on the
 * configured HDMI channel mode
 */
/*static uint32_t csd_dev_afe_get_hdmi_ca_from_dev_info( uint32 channel_mode,
                                                       uint16_t *channel_allocation )
{
  uint32_t rc = CSD_EOK;

  switch (channel_mode)
  {
    case ACDB_CHMODE_HDMI_STEREO:
      *channel_allocation = 0x00;
      break;
    case ACDB_CHMODE_HDMI_3_1:
      *channel_allocation = 0x03;
      break;
    case ACDB_CHMODE_HDMI_5_1:
      *channel_allocation = 0x09;
      break;
    case ACDB_CHMODE_HDMI_7_1:
      *channel_allocation = 0x13;
      break;
    default:
      *channel_allocation = 0x00;
      rc = CSD_ENOTFOUND;
      break;
  }
  return rc;
}*/

static int32_t afe_get_aif_cfg_data_param_id_size( uint16_t afe_port_id,
                                                   uint32_t *param_id,
                                                   uint16_t *param_size )
{
  int32_t rc = CSD_EOK;

  if (NULL == param_size || NULL == param_id)
  {
    CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_cfg_data_param_id_size: "
            "Null PARAM param_size[%p],param_id[%p]",
            ((void*)param_size),((void*)param_id));
    rc = CSD_EBADPARAM;
  }
  else
  {
    switch (afe_port_id)
    {
      case AFE_PORT_ID_PRIMARY_MI2S_RX:
      case AFE_PORT_ID_PRIMARY_MI2S_TX:
      case AFE_PORT_ID_SECONDARY_MI2S_RX:
      case AFE_PORT_ID_SECONDARY_MI2S_TX:
      case AFE_PORT_ID_TERTIARY_MI2S_RX:
      case AFE_PORT_ID_TERTIARY_MI2S_TX:
      case AFE_PORT_ID_QUATERNARY_MI2S_RX:
      case AFE_PORT_ID_QUATERNARY_MI2S_TX:
      case AUDIO_PORT_ID_I2S_RX:
      {
        *param_id = AFE_PARAM_ID_I2S_CONFIG;
        *param_size = sizeof(afe_param_id_i2s_cfg_t);
        break;
      }
      case AFE_PORT_ID_RT_PROXY_PORT_001_RX:
      case AFE_PORT_ID_RT_PROXY_PORT_001_TX:
      {
        *param_id = AFE_PARAM_ID_RT_PROXY_CONFIG;
        *param_size = sizeof(afe_param_id_rt_proxy_port_cfg_t);
        break;
      }
      case AFE_PORT_ID_PRIMARY_PCM_RX:
      case AFE_PORT_ID_PRIMARY_PCM_TX:
      case AFE_PORT_ID_SECONDARY_PCM_RX:
      case AFE_PORT_ID_SECONDARY_PCM_TX:
      {
        *param_id = AFE_PARAM_ID_PCM_CONFIG;
        *param_size = sizeof(afe_param_id_pcm_cfg_t);
        break;
      }
      case AFE_PORT_ID_MULTICHAN_HDMI_RX:
      {
        *param_id = AFE_PARAM_ID_HDMI_CONFIG;
        *param_size = sizeof(afe_param_id_hdmi_multi_chan_audio_cfg_t);
        break;
      }
      case AFE_PORT_ID_DIGITAL_MIC_TX:
      {
        *param_id = AFE_PARAM_ID_DIGI_MIC_CONFIG;
        *param_size = sizeof(afe_param_id_digi_mic_cfg_t);
        break;
      }
      case AFE_PORT_ID_SLIMBUS_RANGE_START:
      {
        *param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
        *param_size = sizeof(afe_param_id_slimbus_cfg_t);
        break;
      }
      case AFE_PORT_ID_PSEUDOPORT_01:
      case AFE_PORT_ID_PSEUDOPORT_02:
      case AFE_PORT_ID_VOICE_RECORD_RX:
      case AFE_PORT_ID_VOICE_RECORD_TX:
      case AFE_PORT_ID_VOICE_PLAYBACK_TX:
      {
        *param_id = AFE_PARAM_ID_PSEUDO_PORT_CONFIG;
        *param_size = sizeof(afe_param_id_pseudo_port_cfg_t);
        break;
      }
      default:
      {
        CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_cfg_data_param_id_size: "
                "Unknown afe port id[%d]",afe_port_id);
        rc = CSD_EUNSUPPORTED;
        break;
      }
    }
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_q6_afe_send_command

  DESCRIPTION   Send message to DSP using ATL

  DEPENDENCIES  

  PARAMETERS    @param msg : message to send
                @param is_sync : synchronous or not

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static int32_t csd_q6_afe_send_command(struct q6msg *msg, bool_t is_sync)
{
  int32_t rc = CSD_EOK;
  uint32_t size = 0;
  struct q6msg* result = NULL;

  csd_malloc(sizeof(struct q6msg),(void**) &result);
  if( result == NULL )
  {
     CSD_MSG(CSD_PRIO_ERROR, "csd_q6_afe_send_command: "
              "Unable to malloc size %d",sizeof(struct q6msg));
     return CSD_ENORESOURCE;
  }

  csd_memset( result, 0, sizeof(struct q6msg));
  rc = csd_atl_send(msg, result, &size, is_sync);

  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_q6_afe_send_command: "
            "Can not send command [0x%x] to dsp, err [%d]",
            msg->hdr.opcode, rc);
  }
  else
  {
    if( result->u.rsp.status != 0 )
    {
      rc = CSD_EQADSP;
      CSD_MSG(CSD_PRIO_ERROR, "csd_q6_afe_send_command: "
              "AFE sending command [0x%x] failed, err [%d]",
               msg->hdr.opcode, result->u.rsp.status);
    }
    else
    {
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_q6_afe_send_command: "
              "AFE command sent [0x%x]", msg->hdr.opcode);
    }
  }
  csd_free(result);
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_check_config_sanity

  DESCRIPTION   Verify sanity afe configurations
                 1 - Check for Direction matches
                 [Exception: RX afe port could be used for looping back mixed
                             audio]
                 2 - Check for Device type and port used
                 3 - Check for invalid NONE channel configuration for AFE
                 4 - TODO: Add more sanity checks (if any)

  DEPENDENCIES

  PARAMETERS    @param dev : a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
static int32_t csd_dev_afe_check_config_sanity(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;

  if( (dev->dev_info.direction == CSD_DEV_RX_DEVICE) &&
      (dev->dev_info.afe_port_id%2 == 1) )
  {
    /* Device direction is RX but afe isn't */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_check_config_sanity: "
            "Afe port is TX while dev is RX");
    rc = CSD_EBADPARAM;
  }

  if( (dev->dev_info.direction == CSD_DEV_TX_DEVICE) &&
      (dev->dev_info.afe_port_id%2 == 0) )
  {
    /* Device direction is TX but afe isn't */
    /* Some applications uses EC Ref tap point in rx port */
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_afe_check_config_sanity: "
            "Afe port is RX while dev is TX,"
            "[Valid for simulating a real device for audio mixing]");
  }

  if( ACDB_CHANNEL_NONE == dev->dev_info.channel_config )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
            "Invalid channel [%u] configuration for dev [%u]",
            dev->dev_info.channel_config, dev->dev_id);
    rc = CSD_EBADPARAM;
  }

  /* Using device type, validate AFE port lies within range */
  switch(dev->dev_info.interface_type)
  {
    case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
      if( !BETWEEN( dev->dev_info.afe_port_id,
                    AFE_PORT_ID_SLIMBUS_RANGE_START,
                    AFE_PORT_ID_SLIMBUS_RANGE_END ) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                "Slimbus device having port [0x%x] not "
                "within slimbus port range", dev->dev_info.afe_port_id );
        rc = CSD_EBADPARAM;
      }
      break;
    case ACDB_DEVICE_TYPE_I2S_PARAM:
    case ACDB_DEVICE_TYPE_PCM_PARAM:
      if( !BETWEEN( dev->dev_info.afe_port_id,
                    AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START,
                    AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END ) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                "I2S/PCM device having port [0x%x] not "
                "within I2S/PCM port range", dev->dev_info.afe_port_id );
        rc = CSD_EBADPARAM;
      }
      break;
    case ACDB_DEVICE_TYPE_VIRTUAL_PARAM:
      if( !BETWEEN( dev->dev_info.afe_port_id,
                    AFE_PORT_ID_PSEUDOPORT_RANGE_START,
                    AFE_PORT_ID_PSEUDOPORT_RANGE_END ) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                "Virtual device having port [0x%x] not "
                "within Pseudo port range", dev->dev_info.afe_port_id );
        rc = CSD_EBADPARAM;
      }
      break;
    case ACDB_DEVICE_TYPE_HDMI_PARAM:
      if( AFE_PORT_ID_MULTICHAN_HDMI_RX != dev->dev_info.afe_port_id )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                "HDMI device using port [0x%x] not "
                "a valid hdmi port", dev->dev_info.afe_port_id );
        rc = CSD_EBADPARAM;
      }
      break;
    case ACDB_DEVICE_TYPE_DIGITAL_MIC_PARAM:
      if( AFE_PORT_ID_DIGITAL_MIC_TX != dev->dev_info.afe_port_id )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                "DMIC device using port [0x%x] not "
                "a valid digital mic port", dev->dev_info.afe_port_id );
        rc = CSD_EBADPARAM;
      }
      break;
    case ACDB_DEVICE_TYPE_INT_BTFM_PARAM:
      if( !BETWEEN( dev->dev_info.afe_port_id,
                    AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START,
                    AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END ) )
       {
         CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                 "BTFM device having port [0x%x] not "
                 "within BTFM port range", dev->dev_info.afe_port_id );
         rc = CSD_EBADPARAM;
       }
       break;
    case ACDB_DEVICE_TYPE_SIMPLE_PARAM:
      {
        /*Simple Param Devices do not have AFE Port ID as Parameter in 
          ACDB Device Configuration structure.*/
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_check_config_sanity: "
                "SIMPLE PARAM devices do not have AFE Port");
        rc = CSD_EBADPARAM;
      }
      break;
    case ACDB_DEVICE_TYPE_RT_PROXY_PARAM:
      if( !BETWEEN( dev->dev_info.afe_port_id,
                    AFE_PORT_ID_RT_PROXY_PORT_RANGE_START,
                    AFE_PORT_ID_RT_PROXY_PORT_RANGE_END ) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
                "RT PROXY device having port [0x%x] not "
                "within rt proxy port range", dev->dev_info.afe_port_id );
        rc = CSD_EBADPARAM;
      }
      break;

    default:
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_check_config_sanity: "
              "Unknown device type [0x%x]",
              dev->dev_info.interface_type );
      rc = CSD_EBADPARAM;
      break;
  }

  return rc;
}

/*=============================================================================

  FUNCTION      afe_set_and_send_cfg_data

  DESCRIPTION   Send afe configuration for a device based on device type

  DEPENDENCIES

  PARAMETERS    @param dev : a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

 =============================================================================*/
static int32_t afe_set_and_send_cfg_data(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  uint16_t afeportid = 0;
  struct q6msg *msg = NULL;
  uint32_t msg_len = 0;
  uint32_t param_id = 0;
  uint16_t param_size = 0;

  afe_port_cmd_set_param_v2_t *afe_set_param = NULL;
  afe_port_param_data_v2_t *afe_param_data = NULL;
  afe_port_param_data_v2_t *afe_param_data_delay = NULL;
  uint8_t *msg_buf = NULL;
  //afe_param_id_hdmi_multi_chan_audio_cfg_t* hdmi_cfg = NULL;
  afe_param_id_i2s_cfg_t* i2s_cfg = NULL;
  afe_param_id_pcm_cfg_t* pcm_cfg = NULL;
  //afe_param_id_digi_mic_cfg_t* digimic_cfg = NULL;
  //afe_param_id_slimbus_cfg_t* sb_cfg = NULL;
  //afe_param_id_pseudo_port_cfg_t* ps_cfg = NULL;
  //afe_param_id_rt_proxy_port_cfg_t *rt_cfg = NULL;
  afe_param_id_device_hw_delay_t *hw_delay = NULL;

  /* Configuration Sanity Check */
  rc = csd_dev_afe_check_config_sanity(dev);

  if( CSD_EOK == rc )
  {
    afeportid = dev->dev_info.afe_port_id;
    if (ACDB_DEVICE_TYPE_SLIMBUS_PARAM == dev->dev_info.interface_type)
    {
      afeportid = AFE_PORT_ID_SLIMBUS_RANGE_START;
    }
    rc = afe_get_aif_cfg_data_param_id_size(afeportid, &param_id, &param_size);
  }

  if( CSD_EOK == rc )
  {
    msg_len = ( sizeof(struct aprv2_packet_t)
                       + sizeof(afe_port_cmd_set_param_v2_t)
                       + sizeof(afe_port_param_data_v2_t)
                       + param_size
                       + sizeof(afe_port_param_data_v2_t)
                       + sizeof(afe_param_id_device_hw_delay_t));

	csd_malloc(msg_len, (void**) &msg);
    if ( msg == NULL )
    {
      CSD_MSG(CSD_PRIO_ERROR,"Unable to allocation memory[%d]",msg_len);
      rc = CSD_ENORESOURCE;
    }
  }
  
  if( CSD_EOK == rc && NULL!=msg)
  {
    csd_memset(msg, 0, msg_len);
    get_common_q6_afe_msg_hdr( (struct aprv2_packet_t*) msg, AFE_PORT_CMD_SET_PARAM_V2);
    Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

    /*Prepare the Message Here */
    msg_buf = (uint8_t*) msg;
    afe_set_param = (afe_port_cmd_set_param_v2_t*)( msg_buf + sizeof(struct aprv2_packet_t) );
    afe_param_data = (afe_port_param_data_v2_t*)(afe_set_param + 1);
    msg_buf = (uint8_t*)(afe_param_data + 1);
    afe_param_data_delay = (afe_port_param_data_v2_t*)(msg_buf + param_size);

    /* Fill the param data for set param command */
    msg->u.afe_set_param_v2.param_data.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
    msg->u.afe_set_param_v2.set_param.mem_map_handle = NULL;

    afe_set_param->payload_size = sizeof(afe_port_param_data_v2_t) + param_size +
                                  sizeof(afe_port_param_data_v2_t) +
                                  sizeof(afe_param_id_device_hw_delay_t);
    afe_set_param->payload_address_lsw = NULL;
    afe_set_param->payload_address_msw = NULL;
    afe_set_param->mem_map_handle = NULL;
    afe_set_param->port_id = dev->dev_info.afe_port_id;
    afe_param_data->module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
    afe_param_data->param_id = param_id;
    afe_param_data->param_size = param_size;

    afe_param_data_delay->module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
    afe_param_data_delay->param_id = AFE_PARAM_ID_DEVICE_HW_DELAY;
    afe_param_data_delay->param_size = sizeof(afe_param_id_device_hw_delay_t);

    switch (afeportid)
    {
      case AFE_PORT_ID_PRIMARY_MI2S_RX:
      case AFE_PORT_ID_PRIMARY_MI2S_TX:
      case AFE_PORT_ID_SECONDARY_MI2S_RX:
      case AFE_PORT_ID_SECONDARY_MI2S_TX:
      case AFE_PORT_ID_TERTIARY_MI2S_RX:
      case AFE_PORT_ID_TERTIARY_MI2S_TX:
      case AFE_PORT_ID_QUATERNARY_MI2S_RX:
      case AFE_PORT_ID_QUATERNARY_MI2S_TX:
      case AUDIO_PORT_ID_I2S_RX:
      {
        const csd_acdb_dev_i2s_info_t* dev_i2s = &dev->dev_info.u.i2s_dev_info;
        i2s_cfg = (afe_param_id_i2s_cfg_t*) ( afe_param_data + 1 );
        /* I2S afe Configuration from csd client */
        i2s_cfg->i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
        i2s_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;
        /* I2S afe Configuration from acdb */
        i2s_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
        i2s_cfg->channel_mode = (uint16_t) dev_i2s->afe_channel_mode;
        i2s_cfg->mono_stereo = (uint16_t) dev_i2s->afe_mono_stereo;
        i2s_cfg->ws_src = (uint16_t) dev_i2s->afe_word_sync_src_select;
        i2s_cfg->data_format = (uint16_t) dev_i2s->afe_data_type;
        break;
      }
	  /*
      case AFE_PORT_ID_RT_PROXY_PORT_001_RX:
      case AFE_PORT_ID_RT_PROXY_PORT_001_TX:
       Unni : Do we need to add these
      case AFE_PORT_ID_RT_PROXY_PORT_002_RX:
      case AFE_PORT_ID_RT_PROXY_PORT_002_TX:
      case AFE_PORT_ID_RT_PROXY_PORT_003_RX:
      case AFE_PORT_ID_RT_PROXY_PORT_003_TX: */
    /*  {
        const csd_dev_rt_proxy_port_cfg_t* dev_rt = &dev->cfg_param.rt_proxy_port_cfg;
        rt_cfg = (afe_param_id_rt_proxy_port_cfg_t *) ( afe_param_data + 1 );
          
        rt_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;
        rt_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
         
        rt_cfg->rt_proxy_cfg_minor_version = AFE_API_VERSION_RT_PROXY_CONFIG;
        rt_cfg->interleaved = (uint16_t) dev_rt->interleaved;
        rt_cfg->frame_size = (uint16_t) dev_rt->frame_size;
        rt_cfg->jitter_allowance = (uint16_t) dev_rt->jitter_allowance;
        rt_cfg->low_water_mark = (uint16_t) dev_rt->low_water_mark;
        rt_cfg->high_water_mark = (uint16_t) dev_rt->high_water_mark;
        rt_cfg->num_channels = (uint16_t) dev_rt->num_channels;
        break;
      }*/
      /* suns: need to check if afeportid here is pseudo port or proxy ports.
       * and how we can merge this portion.
       * currently RT proxy is not supported on MDM */
      case AFE_PORT_ID_PRIMARY_PCM_RX:
      case AFE_PORT_ID_PRIMARY_PCM_TX:
      case AFE_PORT_ID_SECONDARY_PCM_RX:
      case AFE_PORT_ID_SECONDARY_PCM_TX:
      {
        uint16_t iter = 0;
        const csd_acdb_dev_pcm_info_t* dev_pcm = &dev->dev_info.u.pcm_dev_info;
        pcm_cfg = (afe_param_id_pcm_cfg_t*) ( afe_param_data + 1 );
        pcm_cfg->pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
        /* PCM afe Configuration from client */
        pcm_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
        pcm_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;
        /* PCM afe Configuration from acdb */
        pcm_cfg->aux_mode = (uint16_t) dev_pcm->afe_aux_mode;
        pcm_cfg->sync_src = (uint16_t) dev_pcm->afe_sync_src_select;
        pcm_cfg->frame_setting = (uint16_t) dev_pcm->afe_frame_setting;;
        pcm_cfg->quant_type = (uint16_t) dev_pcm->afe_quant_type;
        pcm_cfg->ctrl_data_out_enable = (uint16_t) dev_pcm->afe_data_out_enable_ctrl;
        pcm_cfg->num_channels = (uint16_t) dev->dev_info.channel_config;
        for (iter = 0; iter < pcm_cfg->num_channels; iter++)
        {
          pcm_cfg->slot_number_mapping[iter] =
              (uint16_t) dev_pcm->afe_slot_number_mapping[iter];
        }
        break;
      }/*
      case AFE_PORT_ID_MULTICHAN_HDMI_RX:
      {
        hdmi_cfg = (afe_param_id_hdmi_multi_chan_audio_cfg_t*) ( afe_param_data + 1 );

        hdmi_cfg->hdmi_cfg_minor_version = AFE_API_VERSION_HDMI_CONFIG;
         hdmi rx afe Configuration from csd client 
        hdmi_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
        hdmi_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;
        hdmi_cfg->reserved = (uint16_t) 0x00;
        if( (CSD_DEV_HDMI_MULTICH_SET & dev->cache_cfg_param.control_params) )
        {
          // Override with User defined configuration values 
          hdmi_cfg->data_type = (uint16_t) dev->cache_cfg_param
                                            .hdmi_multich_cfg.data_type;
          hdmi_cfg->channel_allocation = (uint16_t) dev->cache_cfg_param
                            .hdmi_multich_cfg.channel_allocation; 
        }
        else
        {
          uint16_t channel_allocation = 0;
          
          hdmi_cfg->data_type = (uint16_t) dev->dev_info.u.hdmi_dev_info
                                                          .afe_data_type;
          csd_dev_afe_get_hdmi_ca_from_dev_info(
                                 dev->dev_info.u.hdmi_dev_info.afe_channel_mode,
                                 &channel_allocation);
          hdmi_cfg->channel_allocation = channel_allocation;
        }
        break;
      }*/
      /*case AFE_PORT_ID_DIGITAL_MIC_TX:
      {
        const csd_acdb_dev_dmic_info_t* dev_dmic = &dev->dev_info.u.dmic_dev_info;
        digimic_cfg = (afe_param_id_digi_mic_cfg_t*)( afe_param_data + 1 );
        digimic_cfg->digi_mic_cfg_minor_version = AFE_API_VERSION_DIGI_MIC_CONFIG;
        
        digimic_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
        digimic_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;

        digimic_cfg->channel_mode = (uint16_t) dev_dmic->afe_channel_mode;
        break;
      }
      case AFE_PORT_ID_SLIMBUS_RANGE_START:
      {
        
        uint16_t i = 0;
        const csd_acdb_dev_slimbus_info_t* sb_info = &dev->dev_info.u.slimbus_dev_info;
        sb_cfg =  (afe_param_id_slimbus_cfg_t*)( afe_param_data + 1 );

        sb_cfg->sb_cfg_minor_version = AFE_API_VERSION_SLIMBUS_CONFIG;
        
         sb_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
        sb_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;
        
        sb_cfg->slimbus_dev_id = (uint16_t) sb_info->afe_slimbus_dev_id;
        sb_cfg->data_format = (uint16_t) sb_info->afe_data_format;
        sb_cfg->num_channels = (uint16_t) dev->dev_info.channel_config;
        for( i = 0; i < dev->dev_info.channel_config; i++ )
        {
          sb_cfg->shared_channel_mapping[i] = (uint8_t) (sb_info->afe_shared_channel[i]);
        }
        break;
      }
      case AFE_PORT_ID_PSEUDOPORT_01:
      case AFE_PORT_ID_PSEUDOPORT_02:
      case AFE_PORT_ID_VOICE_RECORD_RX:
      case AFE_PORT_ID_VOICE_RECORD_TX:
      case AFE_PORT_ID_VOICE_PLAYBACK_TX:
      {
        const csd_acdb_dev_virtual_info_t* dev_virtual = &dev->dev_info.u.virtual_dev_info;
        ps_cfg = (afe_param_id_pseudo_port_cfg_t*)( afe_param_data + 1 );
        ps_cfg->pseud_port_cfg_minor_version = AFE_API_VERSION_PSEUDO_PORT_CONFIG;
        
        ps_cfg->bit_width = (uint16_t) dev->curr_attrib.bits_per_sample;
        ps_cfg->sample_rate = (uint32_t) dev->curr_attrib.sample_rate;
        
        ps_cfg->num_channels = (uint16_t) dev->dev_info.channel_config;
        ps_cfg->data_format = (uint16_t) dev_virtual->afe_data_type; 
        ps_cfg->timing_mode = (uint16_t) dev_virtual->afe_timing_mode;

        break;
      }*/
      default:
        rc = CSD_EUNSUPPORTED;
        break;
    }

    hw_delay = (afe_param_id_device_hw_delay_t*)(afe_param_data_delay + 1);
    hw_delay->device_hw_delay_minor_version = AFE_API_VERSION_DEVICE_HW_DELAY;
    hw_delay->delay_in_us = dev->av_sync_hw_delay;

    if (CSD_EOK == rc && msg)
    {
      CSD_MSG(CSD_PRIO_HIGH,"afe_set_and_send_cfg_data: "
              "Sending config data and avsync hw delay data");
      rc = csd_q6_afe_send_command(msg, TRUE);
    }
    csd_free(msg);
  }
  return rc;
}

typedef enum csd_spkr_device_cfg
{
  FBSP_MODE_RX_CFG = 0x1,
  VI_PROC_TX_CFG = 0x2,
  SPKR_CALIB_MODE_TX_CFG = 0x3
}csd_spkr_device_cfg_t;

/*=============================================================================

  FUNCTION      csd_dev_afe_get_fb_spkr_prot_calibrated_r0

  DESCRIPTION   Get calibrated R0 for the tx device.

  DEPENDENCIES  Device passed should be in enabled and running state.

  PARAMETERS    @param a_dev : Pointer to device record.
                @param r0_q24: Pointer to r0 (Q24 Value).

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_afe_get_fb_spkr_prot_calibrated_r0(const csd_dev_record_t *dev,
                                                   int32_t *r0_q24)
{
  int32_t rc = CSD_EOK;
  /*struct q6msg *msg = NULL;
  struct q6msg *result = NULL;
  uint32_t size = 0;

  afe_port_cmd_get_param_v2_t *afe_get_param  = NULL;
  aprv2_packet_t *afe_msg_hdr = NULL;

  if( dev == NULL || r0_q24 == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
            "Invalid [NULL] Argument dev[0x%p], r0_q24[0x%p]",
            ((void*)dev), ((void*)r0_q24));
    return CSD_EBADPARAM;
  }

  
  csd_malloc(sizeof(struct q6msg),(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
            "Failed to allocate memory size of (%d)!",
            sizeof(struct q6msg));
    return CSD_ENORESOURCE;
  }
  csd_memset(msg, 0, sizeof(*msg));

  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_PORT_CMD_GET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, (Q6MSG_PACKED_HEADER_SIZE +
                                          sizeof(afe_port_cmd_get_param_v2_t)));

  
  afe_msg_hdr       = (aprv2_packet_t*)msg;
  afe_get_param     = (afe_port_cmd_get_param_v2_t*)(afe_msg_hdr + 1);

  afe_get_param->payload_size         = sizeof(struct q6msg_afe_get_params_rsp);
  afe_get_param->payload_address_lsw  = 0;
  afe_get_param->payload_address_msw  = 0;
  afe_get_param->mem_map_handle       = NULL;
  afe_get_param->port_id              = dev->dev_info.afe_port_id;
  afe_get_param->module_id   			= AFE_MODULE_FB_SPKR_PROT_VI_PROC;
  afe_get_param->param_id    			= AFE_PARAM_ID_CALIB_RES_CFG;

  csd_malloc(sizeof(struct q6msg),(void**) &result);
  if( result == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
            "Unable to malloc size[%d]", sizeof(struct q6msg));
    rc = CSD_ENORESOURCE;
  }
  else
  {
    csd_memset( result, 0, sizeof(*result) );
    
    rc = csd_atl_send( msg, result, &size, TRUE );
    if( CSD_EOK != rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
              "Can not send command (0x%x) to dsp, rc(0x%x)",
              msg->hdr.opcode, rc);
    }
    else
    {
      if( result->u.rsp_w_data.status != 0 )
      {
        rc = CSD_EQADSP;
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
                "AFE sending command (0x%x) failed, rc(0x%x)",
                msg->hdr.opcode, result->u.rsp_w_data.status);
      }
      else
      {
        afe_fbsp_calib_res_cfg_v1_t calibrated_data = {0,};
        csd_memscpy((void*)&calibrated_data, sizeof(calibrated_data),
                    &(result->u.rsp_w_data.u.afe_rsp.u.fbsp_calib_res_cfg),
                    sizeof(result->u.rsp_w_data.u.afe_rsp.u.fbsp_calib_res_cfg));

        CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
                "AFE command rsp recieved (0x%x)", result->hdr.opcode);

        if( AFE_MODULE_FB_SPKR_PROT_VI_PROC == result->u.rsp_w_data.u.afe_rsp.param_data.module_id &&
          AFE_PARAM_ID_CALIB_RES_CFG == result->u.rsp_w_data.u.afe_rsp.param_data.param_id)
        {
          CSD_MSG(CSD_PRIO_HIGH, "csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
                  "AFE calibrated_data.th_vi_ca_state (0x%x)",
                  calibrated_data.th_vi_ca_state);
          CSD_MSG(CSD_PRIO_HIGH, "csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
                  "AFE calibrated_data.r0_cali_q24 (0x%x)",
                  calibrated_data.r0_cali_q24);
           0: Reserved
          1: Calibration is in Progress
          2: Calibration is successful
          3: Calibration is failed
          switch(calibrated_data.th_vi_ca_state)
          {
          case 0:
            *r0_q24 = 0;
            rc = CSD_EQADSP;
            break;
          case 1:
            *r0_q24 = 0;
            rc = CSD_ENOTREADY;
            break;
          case 2:
            *r0_q24 = calibrated_data.r0_cali_q24;
            rc = CSD_EOK;
            break;
          case 3:
            *r0_q24 = 0;
            rc = CSD_EQADSP;
            break;
          };
        }
        else
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
                  "AFE command rsp Wrong - Module Id (0x%x)",
                  result->u.rsp_w_data.u.afe_rsp.param_data.module_id);
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_get_fb_spkr_prot_calibrated_r0: "
                  "AFE command rsp Wrong - Param Id (0x%x)",
                  result->u.rsp_w_data.u.afe_rsp.param_data.param_id);
          rc = CSD_EUNEXPECTED;
        }
      }
    }
  }

  csd_free(msg);
  csd_free(result);*/
  return rc;
}


/*=============================================================================

  FUNCTION      afe_get_aif_additional_spkr_protection_cfg_data

  DESCRIPTION   Get Additional Speaker Protection Cfg Data to be passed to AFE

  DEPENDENCIES  Device passed should be in enabled and running state.

  PARAMETERS    dev : Pointer to device record.
                msg : pointer reference to return msg
                mode : FBSP_MODE_RX_CFG/VI_PROC_TX_CFG/SPKR_CALIB_MODE_TX_CFG

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

 =============================================================================*/
/*static int32_t afe_get_aif_additional_spkr_protection_cfg_data (const csd_dev_record_t *dev,
                                                                struct q6msg **msg,
                                                                csd_spkr_device_cfg_t mode)
{
  int32_t rc = CSD_EOK;
  
  struct q6msg *temp_msg = NULL;
  uint32_t msg_len = 0;
  uint32_t module_id = 0;
  uint32_t param_id  = 0;
  uint16_t param_size = 0;

  afe_port_cmd_set_param_v2_t *afe_set_param  = NULL;
  afe_port_param_data_v2_t *afe_param_data = NULL;
  uint8_t *msg_buf = NULL;

  afe_fbsp_mode_rx_cfg_v1_t* fbsp_mode_rx_cfg = NULL;
  afe_fbsp_mode_vi_proc_cfg_v1_t* vi_proc_mode_cfg = NULL;
  afe_fbsp_spkr_calib_vi_proc_cfg_v1_t* spkr_calib_tx_cfg = NULL;

  if( msg == NULL || NULL == dev)
  {
    CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_additional_spkr_protection_cfg_data: "
            "Invalid msg[0x%p], dev[0x%p]", ((void*)msg), ((void*)dev));
    return CSD_EBADPARAM;
  }

  if( NULL != *msg )
  {
    CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_additional_spkr_protection_cfg_data: "
            "Already allocated pointer msg [0x%p]", ((void*)*msg));
    return CSD_EBADPARAM;
  }

  if(mode == FBSP_MODE_RX_CFG)
  {
    module_id = AFE_MODULE_FB_SPKR_PROT_RX;
    param_id = AFE_PARAM_ID_FBSP_MODE_RX_CFG;
    param_size = sizeof(afe_fbsp_mode_rx_cfg_v1_t);
  }
  else if(mode == VI_PROC_TX_CFG)
  {
    module_id = AFE_MODULE_FB_SPKR_PROT_VI_PROC;
    param_id = AFE_PARAM_ID_MODE_VI_PROC_CFG;
    param_size = sizeof(afe_fbsp_mode_vi_proc_cfg_v1_t);
  }
  else if(mode == SPKR_CALIB_MODE_TX_CFG)
  {
    module_id = AFE_MODULE_FB_SPKR_PROT_VI_PROC;
    param_id = AFE_PARAM_ID_SPKR_CALIB_VI_PROC_CFG;
    param_size = sizeof(afe_fbsp_spkr_calib_vi_proc_cfg_v1_t);
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR, "afe_get_aif_additional_spkr_protection_cfg_data: "
            "Unsupported mode [0x%x]", mode);
    return CSD_EBADPARAM;
  }

  
  msg_len = ( sizeof(struct aprv2_packet_t) +
              +
            sizeof(afe_port_cmd_set_param_v2_t) +
            sizeof(afe_port_param_data_v2_t) +
            param_size);

  csd_malloc(msg_len,(void**) &temp_msg);
  if( temp_msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "afe_get_aif_additional_spkr_protection_cfg_data: "
            "Unable to malloc size[%d]", sizeof(struct q6msg));
    return CSD_ENORESOURCE;
  }

  csd_memset(temp_msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)temp_msg,AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(temp_msg->hdr.header, msg_len);

  
  msg_buf        = (uint8_t*)temp_msg;
  afe_set_param  = (afe_port_cmd_set_param_v2_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data = (afe_port_param_data_v2_t*)(afe_set_param+1);

  afe_set_param->payload_size        = sizeof(afe_port_param_data_v2_t) + param_size;
  afe_set_param->payload_address_lsw = NULL;
  afe_set_param->payload_address_msw = NULL;
  afe_set_param->mem_map_handle      = NULL;
  afe_set_param->port_id             = dev->dev_info.afe_port_id;

  afe_param_data->module_id  = module_id;
  afe_param_data->param_id   = param_id;
  afe_param_data->param_size = param_size;

  switch(mode)
  {
  case FBSP_MODE_RX_CFG:
    {
      if (dev->dev_info.direction == CSD_DEV_RX_DEVICE)
      {
        fbsp_mode_rx_cfg = (afe_fbsp_mode_rx_cfg_v1_t*)(afe_param_data+1);
        fbsp_mode_rx_cfg->fbsp_mode_rx_cfg_minor_version = AFE_API_VERSION_FBSP_MODE_RX_CFG;
        fbsp_mode_rx_cfg->mode = 0;

        if(dev->cache_cfg_param.spkr_calib_cfg.spkr_mode == CSD_CALIBRATION)
        {
          fbsp_mode_rx_cfg->mode = 1;
        }
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR, "afe_get_aif_additional_spkr_protection_cfg_data: "
                "Direction not RX while mode FBSP_MODE_RX_CFG");
        rc = CSD_EBADPARAM;
      }
      break;
    }
  case SPKR_CALIB_MODE_TX_CFG:
    {
      if (dev->dev_info.direction == CSD_DEV_TX_DEVICE)
      {
        spkr_calib_tx_cfg = (afe_fbsp_spkr_calib_vi_proc_cfg_v1_t*)(afe_param_data + 1);
        spkr_calib_tx_cfg->spkr_calib_vi_proc_cfg_minor_version = AFE_API_VERSION_SPKR_CALIB_VI_PROC_CFG;
        spkr_calib_tx_cfg->r0_cali_q24 = dev->cache_cfg_param.spkr_calib_cfg.r0_q24;
        spkr_calib_tx_cfg->t0_cali_q6 = dev->cache_cfg_param.spkr_calib_cfg.t0_q6;
        spkr_calib_tx_cfg->reserved = 0;
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR, "afe_get_aif_additional_spkr_protection_cfg_data: "
                "Direction not TX while mode SPKR_CALIB_MODE_TX_CFG");
        rc = CSD_EBADPARAM;
      }
      break;			
    }
  case VI_PROC_TX_CFG:
    {
      if (dev->dev_info.direction == CSD_DEV_TX_DEVICE)
      {
        vi_proc_mode_cfg = (afe_fbsp_mode_vi_proc_cfg_v1_t*)(afe_param_data + 1);
        vi_proc_mode_cfg->mode_vi_proc_cfg_minor_version  = AFE_API_VERSION_MODE_VI_PROC_CFG;
        vi_proc_mode_cfg->cali_mode =  AFE_FBSP_CALIB_MODE_DISABLE;

        if(dev->cache_cfg_param.spkr_calib_cfg.spkr_mode == CSD_CALIBRATION)
        {
          vi_proc_mode_cfg->cali_mode =  AFE_FBSP_CALIB_MODE_ENABLE;
          CSD_MSG(CSD_PRIO_HIGH, "afe_get_aif_additional_spkr_protection_cfg_data: "
                  " VI Calibration Mode");
        }
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR, "afe_get_aif_additional_spkr_protection_cfg_data: "
                "Direction not TX while mode VI_PROC_TX_CFG");
        rc = CSD_EBADPARAM;
      }
      break;
    }

  default:
    CSD_MSG(CSD_PRIO_ERROR, "afe_get_aif_additional_spkr_protection_cfg_data: "
            "Unsupported mode [0x%x]", mode);
    rc = CSD_EUNSUPPORTED;
    break;
  }

  if(CSD_EOK == rc)
  {
    *msg = temp_msg;
  }
  else
  {
    *msg = NULL;
    csd_free(temp_msg);
  }
  return rc;
}
*/
/*=============================================================================

  FUNCTION      afe_get_aif_create_afe_common_table_data_msg

  DESCRIPTION   Create Data message with AFE Common Table Data to be passed to AFE

  DEPENDENCIES  Device passed should be in enabled and running state.

  PARAMETERS    @param a_dev : Pointer to device record.

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

 =============================================================================*/
static int32_t afe_get_aif_create_afe_common_table_data_msg (
                                                  const csd_dev_record_t *dev,
                                                  struct q6msg **msg,
                                                  const uint8_t* afe_data,
                                                  uint32_t afe_data_size)
{
  int32_t rc = CSD_EOK;
  struct q6msg *temp_msg = NULL;
  uint32_t msg_len = 0;
  uint16_t param_size = afe_data_size;

  afe_port_cmd_set_param_v2_t *afe_set_param  = NULL;
  uint8_t *msg_buf = NULL;

  if( NULL == msg || NULL == afe_data || NULL == dev)
  {
    CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_create_afe_common_table_data_msg: "
            "NULL Argument msg[0x%p], afe_data[0x%p], dev[0x%p]",
            ((void*)msg), ((void*)afe_data), ((void*)dev));
    return CSD_EBADPARAM;
  }

  if( NULL != *msg )
  {
    CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_create_afe_common_table_data_msg: "
            "Already allocated pointer[0x%p]", ((void*)*msg));
    return CSD_EBADPARAM;
  }

  /*Allocate and Prepare the Message Param*/
  msg_len = ( sizeof(struct aprv2_packet_t) +
              sizeof(afe_port_cmd_set_param_v2_t) + afe_data_size);
  csd_malloc(msg_len,(void**) &temp_msg);
  if( temp_msg == NULL ) 
  {
    CSD_MSG(CSD_PRIO_ERROR,"afe_get_aif_create_afe_common_table_data_msg: "
            "Cannot allocate size %d",msg_len);
    return CSD_ENORESOURCE;
  }

  csd_memset(temp_msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)temp_msg,AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(temp_msg->hdr.header, msg_len);

  /*Prepare the Message Here */
  msg_buf       = (uint8_t*)temp_msg;
  afe_set_param = (afe_port_cmd_set_param_v2_t*)(msg_buf + sizeof(struct aprv2_packet_t));

  afe_set_param->payload_size         = param_size;
  afe_set_param->payload_address_lsw  = NULL;
  afe_set_param->payload_address_msw  = NULL;
  afe_set_param->mem_map_handle       = NULL;
  afe_set_param->port_id              = dev->dev_info.afe_port_id;

  CSD_MSG(CSD_PRIO_MEDIUM,"afe_get_aif_create_afe_common_table_data_msg: "
          "param_size = (%d)!!!", param_size);
  /*Copy the ACDB Blob*/
  csd_memscpy((msg_buf + sizeof(struct aprv2_packet_t) + sizeof(afe_port_cmd_set_param_v2_t)),
              afe_data_size, afe_data, param_size);

  if(CSD_EOK == rc)
  {
    *msg = temp_msg;
  }
  else
  {
    *msg = NULL;
    csd_free(temp_msg);
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_cfg_fb_speaker_protection_params

  DESCRIPTION   This function sends FB Speaker Protection data to AFE.

  DEPENDENCIES

  PARAMETERS    @param dev : a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_cfg_fb_speaker_protection_params(const csd_dev_record_t *dev)
{
  int32_t rc  = CSD_EOK;
  /* Speaker Protection Device AFE Port is configured */
  /*CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_afe_cfg_fb_speaker_protection_params: "
          "AFE_Port Direction=[0x%x] FB Speaker Protection "
          "Setting Direction Specific APPS Params !!",
    dev->dev_info.direction);

  
  if (dev->dev_info.direction == CSD_DEV_RX_DEVICE)
  {
    struct q6msg *msg = NULL;
    rc = afe_get_aif_additional_spkr_protection_cfg_data (dev, &msg, FBSP_MODE_RX_CFG);
    if(CSD_EOK == rc)
    {
      rc = csd_q6_afe_send_command(msg,TRUE);
      csd_free(msg);
      msg = NULL;
    }
  }
  else if(dev->dev_info.direction == CSD_DEV_TX_DEVICE)
  {
    struct q6msg *msg = NULL;
    rc = afe_get_aif_additional_spkr_protection_cfg_data (dev, &msg, VI_PROC_TX_CFG);
    if(CSD_EOK == rc)
    {
      rc = csd_q6_afe_send_command(msg,TRUE);
      csd_free(msg);
      msg = NULL;
    }

    rc = afe_get_aif_additional_spkr_protection_cfg_data (dev, &msg, SPKR_CALIB_MODE_TX_CFG);
    if(CSD_EOK == rc)
    {
      rc = csd_q6_afe_send_command(msg,TRUE);
      csd_free(msg);
      msg = NULL;
    }
  }

  CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_cfg_fb_speaker_protection_params: "
          "Speaker Protection Parameters AFE End. rc=[0x%x]",rc);
          */
  return rc;
}

/*=============================================================================
  
  FUNCTION      csd_dev_afe_set_common_table

  DESCRIPTION   This function get device common table and set to AFE

  DEPENDENCIES

  PARAMETERS    @param dev : a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_set_common_table(const csd_dev_record_t *dev)
{
  int32_t rc  = CSD_EOK;
  uint8_t* afe_data = NULL;
  uint32_t actual_buf_size = 0;
  uint32_t default_buf_size = 4096;

  /* Allocate for ACDB Buffer */
  csd_malloc(default_buf_size,(void**)&afe_data);
  if ( afe_data == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_set_common_table: "
            "Failed to allocate memory size of (%d)!",
            default_buf_size);
    rc = CSD_ENORESOURCE;
  }
  else
  {
    /*Set AFE Port Data to ADSP*/
    csd_memset(afe_data, 0, default_buf_size);
    rc = csd_acdb_get_afe_common_table_data(dev->dev_id,
                                            dev->curr_attrib.sample_rate,
                                            afe_data, default_buf_size,
                                            &actual_buf_size);
    if(CSD_EOK == rc)
    {
      /*AFE ACDB Related Data*/
      struct q6msg *msg = NULL;
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_afe_set_common_table: "
              " Trying to set AFE common table the "
              "AFE_Port=[0x%x] !!", dev->dev_info.afe_port_id);
      rc = afe_get_aif_create_afe_common_table_data_msg(dev, &msg, afe_data,
                                                       actual_buf_size);
      if(CSD_EOK == rc)
      {
        rc = csd_q6_afe_send_command(msg,TRUE);
        csd_free(msg);
        msg = NULL;
      }
    }
    else if(CSD_ENOTFOUND == rc)
    {
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_afe_set_common_table: "
              " Set AFE common table to "
              "AFE_Port=[0x%x] is skipped!!", dev->dev_info.afe_port_id);
    }
    csd_free(afe_data);
  }

  CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_set_common_table: "
          "Set common table AFE End. rc=[0x%x]",rc);
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_start

  DESCRIPTION   This function starts a AFE HW port or create a pseudo port
                if requested device is a virtual device.

  DEPENDENCIES  

  PARAMETERS    @param dev : a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_start(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  bool_t skip_to_end = FALSE;
  uint16_t index = 0;
  uint32_t port_status = CSD_DEV_AFE_PORT_NOT_CONFIGURED;

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_start: Entering");

  if( dev == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_start: "
            "invalid dev input parameter");
    rc = CSD_EBADPARAM;
  }

  if( (CSD_EOK == rc) && (dev->dev_info.direction & CSD_DEV_TX_DEVICE) &&
      (dev->mask & CSD_DEV_STATUS_ANC_MASK) )
  {
    /* ANC TX path is internal to CODEC, no AFE configuration is needed. */
    rc = CSD_EOK;
    /* skip send AFE config and AFE start cmds */
    skip_to_end = TRUE;
  }

  if( (CSD_EOK == rc) && (!skip_to_end) )
  {
    index = csd_dev_get_port_index(dev->dev_info.afe_port_id);
    if(index < CSD_DEV_PORT_INDICES_MAX)
    {
      port_status = csd_dev_afe_port_info[index].port_status;
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_start: "
              "Invalid index  %d",index);
      rc = CSD_ENORESOURCE;
    }
  }

  if( (CSD_EOK == rc) && (!skip_to_end) &&
      (CSD_DEV_AFE_PORT_NOT_CONFIGURED == port_status) )
  {
    /* Configure the AFE Port */
    CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_start: "
                  "Trying to configure the AFE_Port=[0x%x]!",
                  dev->dev_info.afe_port_id);
    rc = afe_set_and_send_cfg_data(dev);
    if( CSD_EOK == rc )
    {
      uint32_t nreturn = CSD_EOK;
      port_status = CSD_DEV_AFE_PORT_CONFIGURED;
      csd_dev_afe_port_info[index].port_status = CSD_DEV_AFE_PORT_CONFIGURED;

      /*FB Speaker Protection Device*/
      if(dev->cache_cfg_param.control_params & CSD_DEV_SPKR_PROT_SET)
      {
        CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_start: "
          "AFE_Port=[0x%x] FB Speaker Protection Setting Params !!",
          dev->dev_info.afe_port_id);
        nreturn = csd_dev_afe_cfg_fb_speaker_protection_params(dev);
      }
      /* Set AFE common table for all devices */
      if((nreturn != CSD_EOK) && (nreturn != CSD_ENOTFOUND ))
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_start: "
                " AFE_Port=[0x%x] FB Speaker Protection Enabled Failed with Err=[0x%x]!!",
          dev->dev_info.afe_port_id, nreturn);
      }
      else
      {
        nreturn = csd_dev_afe_set_common_table(dev);
        if( nreturn != CSD_EOK )
        {
          CSD_MSG(CSD_PRIO_HIGH, "csd_dev_afe_start: Unable to set common table, rc[0x%x], skipping", nreturn );                          
        }        
      }
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_start: "
              "AFE_Port=[0x%x] Configuration Failed "
              "with Err=[0x%x]!", dev->dev_info.afe_port_id, rc);
    }
  }/* End if Port configuration */

  if( (CSD_EOK == rc) && (!skip_to_end) &&
      (CSD_DEV_AFE_PORT_CONFIGURED == port_status) )
  {
     struct q6msg* msg = NULL;
	 csd_malloc(sizeof(struct q6msg),(void**) &msg);
     if( msg == NULL )
     {
       CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_start: "
               "Failed to allocate memory size of (%d)!",
               sizeof(struct q6msg));
       return CSD_ENORESOURCE;
     }
     csd_memset(msg, 0, sizeof(*msg));
     /* Send HW afe port start  */
     get_common_q6_afe_msg_hdr(&(msg->hdr),AFE_PORT_CMD_DEVICE_START);
     msg->u.afe_port_start.port_id = dev->dev_info.afe_port_id;
     Q6MSG_SET_PACKET_SIZE(msg->hdr.header,(Q6MSG_PACKED_HEADER_SIZE +
                                          sizeof(msg->u.afe_port_start)));
     rc = csd_q6_afe_send_command(msg,TRUE);
     csd_free(msg);
    if( CSD_EOK != rc )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_start: "
              "AFE_Port[0x%x] AFE_PORT_CMD_DEVICE_START"
                    "Failed with rc=[0x%x]!", dev->dev_info.afe_port_id, rc);
    }
    else
    {
      port_status = CSD_DEV_AFE_PORT_ACTIVE;
      csd_dev_afe_port_info[index].port_status = CSD_DEV_AFE_PORT_ACTIVE;
    }
  }

  if( (CSD_EOK == rc) && (!skip_to_end) &&
      (CSD_DEV_AFE_PORT_ACTIVE == port_status) )
  {
    /* AFE port enabled */
    csd_dev_afe_port_info[index].ref_count++;
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_afe_start: "
            "Enabled Port=[0x%x] ref_cnt=[0x%x]!!",
                    dev->dev_info.afe_port_id,
                    csd_dev_afe_port_info[index].ref_count);
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_start: Exiting");

  if( (CSD_EOK == rc)
    && (dev->cache_cfg_param.control_params & CSD_DEV_SPKR_PROT_SET)
    && (dev->dev_info.direction == CSD_DEV_RX_DEVICE) )
  {
    /*Configure/Enable the Feed Back Speaker-Tx AFE Port*/
    csd_dev_record_t* fb_tx_device_record = NULL;
    rc = csd_dev_fb_spkr_prot_get_tx_device_record(dev, &fb_tx_device_record);
    if( (rc == CSD_EOK) && (fb_tx_device_record != NULL) )
    {
      rc = csd_dev_afe_start(fb_tx_device_record);
      if((rc == CSD_EOK) && (dev->cache_cfg_param.spkr_calib_cfg.spkr_mode == CSD_NORMAL) )
      {
        csd_dev_fb_spkr_prot_device_enabled(dev->dev_id);
      }
      csd_free(fb_tx_device_record);
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_start: rc=[0x%x] fb_tx_device_record =[0x%p].",
        rc,((void*)fb_tx_device_record));
    }
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_stop

  DESCRIPTION   This function stop a AFE HW port or destroy a pseudo port
                if requested device is a virtual device.

  DEPENDENCIES  

  PARAMETERS    @param dev : a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_stop(const csd_dev_record_t* dev)
{
  int32_t rc = CSD_EOK;
  bool_t skip_to_end = FALSE;
  uint16_t index = 0;

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_stop: Entering");

  if( dev == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_stop: "
            "invalid a_dev input parameter");
    rc = CSD_EBADPARAM;
  }
  if( CSD_EOK == rc && (dev->dev_info.direction & CSD_DEV_TX_DEVICE)
      && (dev->mask & CSD_DEV_STATUS_ANC_MASK) )
  {
    /* ANC TX path is internal to CODEC, no AFE configuration is needed. */
    rc = CSD_EOK;
    /* skip send AFE config and AFE start cmds */
    skip_to_end = TRUE;
  }

  if( (CSD_EOK == rc) && (!skip_to_end) )
  {
    index = csd_dev_get_port_index(dev->dev_info.afe_port_id);
    if(index >= CSD_DEV_PORT_INDICES_MAX)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_stop: Invalid index  %d",index);
      rc = CSD_ENORESOURCE;
    }
    else
    {
      /* Check for current status and reference count */
      if ( (CSD_DEV_AFE_PORT_ACTIVE != csd_dev_afe_port_info[index].port_status) ||
           (0 == csd_dev_afe_port_info[index].ref_count) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_stop: AFE_PS_PORT [0x%x] "
                "is Not Active [%d] ref_cnt=[%d]!!",
                dev->dev_info.afe_port_id,
                csd_dev_afe_port_info[index].port_status,
                csd_dev_afe_port_info[index].ref_count);
        skip_to_end = TRUE; /* goto end; */
      }
      else
      {
        csd_dev_afe_port_info[index].ref_count--;
        if( csd_dev_afe_port_info[index].ref_count )
        {
          CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_afe_stop: AFE_PS_PORT [0x%x] "
                  "Needs to be Kept Active: Ref Count= [%d]!",
                  dev->dev_info.afe_port_id,
                  csd_dev_afe_port_info[index].ref_count);
          skip_to_end = TRUE; /* goto end; */
        }
      }
    }
  }

  if( (CSD_EOK == rc) && (!skip_to_end) )
  {
    /* Continue to send AFE STOP command. */
    struct q6msg *q6_msg = NULL;

	csd_malloc(sizeof(struct q6msg),(void**) &q6_msg);
    if( q6_msg == NULL )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_stop: "
              "Failed to allocate memory size of (%d)!",
              sizeof(struct q6msg));
      return CSD_ENORESOURCE;
    }

    csd_memset(q6_msg, 0, sizeof(*q6_msg));
    /* Handle HW afe port stop */
    get_common_q6_afe_msg_hdr(&q6_msg->hdr, AFE_PORT_CMD_DEVICE_STOP);
    q6_msg->u.afe_port_stop.port_id = dev->dev_info.afe_port_id;
    Q6MSG_SET_PACKET_SIZE(q6_msg->hdr.header,
                    (Q6MSG_PACKED_HEADER_SIZE+ sizeof(q6_msg->u.afe_port_stop)));

    if (!(csd_handle_is_type_ssr(CSD_OPEN_DEVICE_CONTROL)))
    {
      rc = csd_q6_afe_send_command(q6_msg, TRUE);
    }
    else
    {
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_afe_stop: "
              "AFE_PORT_CMD_DEVICE_STOP for PortId [0x%x] Skipped due to SSR",
              dev->dev_info.afe_port_id);
    }
    csd_free(q6_msg);
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_afe_stop: "
        "AFE_PORT_CMD_DEVICE_STOP for PortId (0x%x) completed with rc=[0x%x]",
        dev->dev_info.afe_port_id, rc);

    if( CSD_EOK == rc )
    {
      /* end if updating AFE port status */
      csd_dev_afe_port_info[index].port_status = CSD_DEV_AFE_PORT_NOT_CONFIGURED;
    }
  } /* end if skip_to_end flag is not set */

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_stop: Exiting [%x]",rc);

  if( (CSD_EOK==rc) &&
    (dev->cache_cfg_param.control_params & CSD_DEV_SPKR_PROT_SET) &&
    (dev->dev_info.direction == CSD_DEV_RX_DEVICE))
  {
    /*Configure/Disable the Feed Back Speaker-Tx AFE Port*/
    csd_dev_record_t* fb_tx_device_record = NULL;
    rc = csd_dev_fb_spkr_prot_get_tx_device_record(dev, &fb_tx_device_record);
    if( (rc == CSD_EOK) && (fb_tx_device_record != NULL) )
    {
      rc = csd_dev_afe_stop(fb_tx_device_record);
      if((rc == CSD_EOK)&& (dev->cache_cfg_param.spkr_calib_cfg.spkr_mode == CSD_NORMAL))
      {
        csd_dev_fb_spkr_prot_device_disabled(dev->dev_id);
      }
      csd_free(fb_tx_device_record);
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_stop: rc=[0x%x] fb_tx_device_record =[0x%p]",
        rc, ((void*)fb_tx_device_record));
    }
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_

  DESCRIPTION   <>

  DEPENDENCIES  This function enable loopback between a TX port and RX port

  PARAMETERS    @param <> : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_enable_loopback(uint16_t dst_afe_id, uint16_t src_afe_id,
                                    bool_t enable_flag, uint16_t mode)
{
  int32_t rc = CSD_EOK;
  struct q6msg *msg = NULL;
  uint32_t msg_len = 0;
  aprv2_packet_t* afe_msg_hdr = NULL;
  afe_port_cmd_set_param_v2_t *afe_set_param = NULL;
  afe_port_param_data_v2_t *afe_param_data = NULL;
  afe_loopback_cfg_v1_t *afe_lb_data = NULL;

  CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_enable_loopback: "
          "Enable AFE Loopback: dst_afe_id=%d  src_afe_id=%d "
          "enable_flag=%d mode=%d", dst_afe_id, src_afe_id, enable_flag, mode);

  msg_len = ( sizeof(struct aprv2_packet_t)
            + sizeof(afe_port_cmd_set_param_v2_t)
            + sizeof(afe_port_param_data_v2_t)
            + sizeof(afe_loopback_cfg_v1_t) );

  csd_malloc(msg_len, (void**) &msg);
  if ( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_enable_loopback: "
            "Failed to allocate memory size of (%d)!",
            msg_len);
    return CSD_ENORESOURCE;
  }

  csd_memset(msg, 0, msg_len);
  /*Prepare the Message Here */
  afe_msg_hdr = (aprv2_packet_t*) msg;
  afe_set_param = (afe_port_cmd_set_param_v2_t*) (afe_msg_hdr + 1);
  afe_param_data = (afe_port_param_data_v2_t*) (afe_set_param + 1);
  afe_lb_data = (afe_loopback_cfg_v1_t*) (afe_param_data + 1);

  get_common_q6_afe_msg_hdr(afe_msg_hdr, AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  afe_set_param->payload_size = sizeof(afe_port_param_data_v2_t) +
                                sizeof(afe_loopback_cfg_v1_t);
  afe_set_param->payload_address_lsw = NULL;
  afe_set_param->payload_address_msw = NULL;
  afe_set_param->mem_map_handle = NULL;
  afe_set_param->port_id = src_afe_id;

  afe_param_data->module_id = AFE_MODULE_LOOPBACK;
  afe_param_data->param_id = AFE_PARAM_ID_LOOPBACK_CONFIG;
  afe_param_data->param_size = sizeof(afe_loopback_cfg_v1_t);

  afe_lb_data->loopback_cfg_minor_version = AFE_API_VERSION_LOOPBACK_CONFIG;
  afe_lb_data->dst_port_id = dst_afe_id;
  afe_lb_data->enable = enable_flag;
  afe_lb_data->routing_mode = ((mode == 0) ? (LB_MODE_DEFAULT) : (mode));
  afe_lb_data->reserved = 0;

  rc = csd_q6_afe_send_command(msg, TRUE);
  /* free mem */
  csd_free(msg);
  if (CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_enable_loopback: "
            "Loopback_V2 command Failed for "
                  "src_port[0x%x], dst_port[0x%x]",
                  src_afe_id, dst_afe_id);
  }

  return rc;
}

static int32_t csd_dev_afe_set_loopback_gain( uint32_t src_tx_afe_id,
                                              uint32_t dst_rx_afe_id,
                                              uint16_t gain )
{
  int32_t rc = CSD_EOK;

  struct q6msg *msg = NULL;
  uint32_t msg_len = 0;
  aprv2_packet_t* hdr = NULL;
  afe_port_cmd_set_param_v2_t *afe_set_param = NULL;
  afe_port_param_data_v2_t *afe_param_data = NULL;
  afe_loopback_gain_per_path_param_t *afe_lb_gain = NULL;

  msg_len = (sizeof(struct aprv2_packet_t)
          + sizeof(afe_port_cmd_set_param_v2_t)
          + sizeof(afe_port_param_data_v2_t)
          + sizeof(afe_loopback_gain_per_path_param_t));

  csd_malloc(msg_len, (void**) &msg);
  if ( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_set_loopback_gain: "
            "Unable to malloc size[%d]", msg_len);
    return CSD_ENORESOURCE;
  }

  csd_memset(msg, 0, msg_len);
  /*Prepare the Message Here */
  hdr = (aprv2_packet_t*) msg;
  afe_set_param = (afe_port_cmd_set_param_v2_t*) (hdr + 1);
  afe_param_data = (afe_port_param_data_v2_t*) (afe_set_param + 1);
  afe_lb_gain = (afe_loopback_gain_per_path_param_t*) (afe_param_data + 1);

  get_common_q6_afe_msg_hdr(hdr, AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  afe_set_param->payload_size = sizeof(afe_port_param_data_v2_t) +
                                sizeof(afe_loopback_gain_per_path_param_t);
  afe_set_param->payload_address_lsw = NULL;
  afe_set_param->payload_address_msw = NULL;
  afe_set_param->mem_map_handle = NULL;
  afe_set_param->port_id = src_tx_afe_id;

  afe_param_data->module_id = AFE_MODULE_LOOPBACK;
  afe_param_data->param_id = AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH;
  afe_param_data->param_size = sizeof(afe_loopback_gain_per_path_param_t);

  afe_lb_gain->rx_port_id = dst_rx_afe_id;
  afe_lb_gain->gain = gain;

  rc = csd_q6_afe_send_command(msg, TRUE);
  /* free mem */
  csd_free(msg);
  if (CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_set_loopback_gain: "
        "Loopback_V2 command Failed for src_port[0x%x], dst_port[0x%x]",
        src_tx_afe_id, dst_rx_afe_id);
  }

  return rc;
}


/*=============================================================================

  FUNCTION      csd_dev_afe_enable_sidetone_iir

  DESCRIPTION   his function enable or disable the IIR Filter

  DEPENDENCIES  

  PARAMETERS    @param <> : uint16_t tx_afe_id
                @param  <> : bool_t enable_flag

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_enable_sidetone_iir(uint16_t tx_afe_id,
                                            bool_t enable_flag)
{
  int32_t rc = CSD_EOK;
  struct q6msg *msg = NULL;
  uint32_t msg_len = 0;
  aprv2_packet_t* afe_msg_hdr = NULL;
  afe_port_cmd_set_param_v2_t *afe_set_param = NULL;
  afe_port_param_data_v2_t *afe_param_data = NULL;
  afe_mod_enable_param_t* afe_enable_param = NULL;

  CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_enable_sidetone_iir: "
          "Enable IIR Filter:  enable_flag = 0x%x", enable_flag);
  msg_len = ( sizeof(struct aprv2_packet_t)
            + sizeof(afe_port_cmd_set_param_v2_t)
            + sizeof(afe_port_param_data_v2_t)
            + sizeof(afe_mod_enable_param_t));

  csd_malloc(msg_len, (void**) &msg);
  if ( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_enable_sidetone_iir: "
            "Unable to malloc size[%d]", msg_len);
    return CSD_ENORESOURCE;
  }

  /* clearing/initializing message */
  csd_memset(msg, 0, msg_len);

  /*Prepare the Message Here */
  afe_msg_hdr = (aprv2_packet_t*) msg;
  afe_set_param = (afe_port_cmd_set_param_v2_t*) (afe_msg_hdr + 1);
  afe_param_data = (afe_port_param_data_v2_t*) (afe_set_param + 1);
  afe_enable_param = (afe_mod_enable_param_t*) (afe_param_data + 1);

  get_common_q6_afe_msg_hdr(afe_msg_hdr, AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  afe_set_param->mem_map_handle = NULL;
  afe_set_param->port_id = tx_afe_id;
  afe_set_param->payload_size = sizeof(afe_port_param_data_v2_t) +
                                sizeof(afe_mod_enable_param_t);

  afe_param_data->module_id = AFE_MODULE_SIDETONE_IIR_FILTER;
  afe_param_data->param_id = AFE_PARAM_ID_ENABLE;
  afe_param_data->param_size = sizeof(afe_mod_enable_param_t);

  afe_enable_param->enable = enable_flag;
  rc = csd_q6_afe_send_command(msg,TRUE);
  csd_free(msg);
  if (CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_enable_sidetone_iir: "
            "Sidetone irr enable command Failed "
                  "for tx_port[0x%x]", tx_afe_id);
  }

  return rc;

}

/*=============================================================================

  FUNCTION      csd_dev_afe_send_sidetone_iir_data

  DESCRIPTION   This function configures the side tone IIR Filter data

  DEPENDENCIES  

  PARAMETERS    @param <> : uint16_t tx_afe_id
                @param  <> : q6msg_config_iir_filter* iir_data

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_send_sidetone_iir_data(uint32_t rx_dev_id,
                                           uint32_t tx_dev_id,
                                           uint16_t tx_afe_id )
{
  int32_t rc = CSD_EOK;
  struct q6msg q6_msg;
  uint32_t iir_filter_size = 0, numshift = 0;
  int32_t* numshiftfactor_ptr = NULL;
  uint32_t numshiftfactor_size = 0;
  uint32_t size = 0;
  struct q6msg_config_sidetone_iir_filter* iir_filter = NULL;

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
  }sidetone_iir_data_t ;
  sidetone_iir_data_t iir_data;


  rc = csd_dev_acdb_get_afe_sidetone_iir_data( rx_dev_id, tx_dev_id,
                                               (void *)&iir_data,
                                               sizeof(sidetone_iir_data_t));

  if ( CSD_EOK == rc )
  {
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_afe_enable_sidetone_iir: "
            "Configure Side tone IIR Filter data: "
                   "tx_afe_id = 0x%x  num_bi_quad_stages:0x%x "
                   ,tx_afe_id,iir_data.num_bi_quad_stages);

    if( iir_data.num_bi_quad_stages <= CSD_Q6MSG_IIR_FILTER_STAGES_MAX )
    {
      csd_memset(&q6_msg, 0, sizeof(q6_msg));
      get_common_q6_afe_msg_hdr(&q6_msg.hdr,AFE_PORT_CMD_SET_PARAM_V2);

      /*Calculate the size of the IIR filter data*/
      /*IIR Filter has 2 bytes num_bi_quad_stages data,
         2 bytes of pre_gain
         20 bytes for each stage IIR filter coefficients
         2 bytes for each stage num shift factors*/
      iir_filter_size = sizeof(uint16_t)/*num_bi_quad_stages */ + \
                        sizeof(uint16_t)/*pre_gain */         +  \
         (CSD_Q6MSG_IIR_FILTER_COEFF_PER_STAGE * (sizeof(int32_t)) * \
                                        iir_data.num_bi_quad_stages) + \
                        ((sizeof(int16_t) * iir_data.num_bi_quad_stages));
      if( iir_data.num_bi_quad_stages & 0x1 )
      {
        /* if num stages are odd number then add 2 bytes padding data*/
        CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_enable_sidetone_iir: "
                "side tone iir_filter num stages are odd so "
                   "adding 2 bytes for padding: %d ",iir_filter_size);
         iir_filter_size += 2;
      }
      CSD_MSG(CSD_PRIO_LOW,"side tone iir_filter size: %d ",iir_filter_size);

      /* Fill the param data for set param command */
      iir_filter = &(q6_msg.u.sidetone_iir_filter);
      iir_filter->set_param.mem_map_handle = NULL;
      iir_filter->set_param.port_id = tx_afe_id;
      iir_filter->set_param.payload_size = sizeof(afe_port_param_data_v2_t)
                                           + iir_filter_size;
      iir_filter->param_data.module_id = AFE_MODULE_SIDETONE_IIR_FILTER;
      iir_filter->param_data.param_id = AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG;
      iir_filter->param_data.param_size = iir_filter_size;
      iir_filter->iir_filter.num_bi_quad_stages = iir_data.num_bi_quad_stages;
      iir_filter->iir_filter.pre_gain = iir_data.pre_gain;

      /* Copy the IIR filter coefficients */
      size = (iir_data.num_bi_quad_stages * CSD_Q6MSG_IIR_FILTER_COEFF_PER_STAGE
              * sizeof(int32_t));
      csd_memscpy( &(iir_filter->iir_filter.filter_coeff_shift),
                   sizeof(iir_filter->iir_filter.filter_coeff_shift),
                   &(iir_data.filter_coeff_shift),
                   size);
      /*Calculate the Num shift factor offset in the IIR filter array */
      numshift = (iir_data.num_bi_quad_stages * \
                             CSD_Q6MSG_IIR_FILTER_COEFF_PER_STAGE );
      /*Get the Num shift factor data address in the IIR filter array */
      numshiftfactor_ptr =
                         &(iir_filter->iir_filter.filter_coeff_shift[numshift]);
      numshiftfactor_size = sizeof(iir_filter->iir_filter.filter_coeff_shift)
                            - (numshift*sizeof(int32_t));


      size = iir_data.num_bi_quad_stages * (sizeof(int16_t));
      /* Copy the IIR Num Shift factor data */
      csd_memscpy( numshiftfactor_ptr, numshiftfactor_size,
                   &(iir_data.filter_coeff_shift[numshift]), size);

      Q6MSG_SET_PACKET_SIZE(q6_msg.hdr.header,
           (Q6MSG_PACKED_HEADER_SIZE+sizeof(afe_port_cmd_set_param_v2_t)+ \
            sizeof(afe_port_param_data_v2_t)+iir_filter_size) );
      rc = csd_q6_afe_send_command(&q6_msg,TRUE);
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_afe_enable_sidetone_iir: "
              "IIR Data send to AFE status:0x%x ", rc);
    }
    else
    {
      rc = CSD_EBADPARAM;
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_enable_sidetone_iir: "
              "Enable Side tone IIR data: "
                     "Invalid input number of stages :0x%x "
                     , iir_data.num_bi_quad_stages);
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_enable_sidetone_iir: "
            "Enable Side tone IIR data: "
                   "Function invoked with NULL parameter ");

  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_enable_sidetone

  DESCRIPTION   This function enable the side tone gain

  DEPENDENCIES  

  PARAMETERS    @param : uint16_t rx_afe_id
                @param : uint16_t tx_afe_id
                @param : uint32_t enable_flag
                @param : uint16_t gains

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_enable_sidetone(uint16_t rx_afe_id, uint16_t tx_afe_id,
    uint32_t enable_flag, uint16_t gains)
{
  int32_t rc = CSD_EOK;
  rc = csd_dev_afe_enable_loopback(rx_afe_id, tx_afe_id, enable_flag, LB_MODE_SIDETONE);
  /* For Gain: Need to apply the Gain Param using AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH::
   * Apply Gain if  the SideTone Enable is sucessful */
  if ( (CSD_EOK == rc) && (enable_flag) )
  {
    rc = csd_dev_afe_set_loopback_gain(tx_afe_id, rx_afe_id, gains);
  }

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_afe_set_lpass_if_clock

  DESCRIPTION   This function configures the lpass interface clock 

  DEPENDENCIES  

  PARAMETERS    @param <> :uint16_t tx_afe_id
                @param <> :afe_param_id_lpaif_clk_cfg_t *clk_cfg

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_set_clock(uint16_t afe_port_id,
                              const afe_param_id_clock_set_t *clk_cfg)
{
  uint32_t msg_len = 0;
  struct q6msg *msg = NULL;
  uint32_t rc = CSD_EOK;
  uint8_t *msg_buf = NULL;
  afe_svc_cmd_set_param_t* afe_set_param;
  afe_svc_param_data_t* afe_param_data;
  afe_param_id_clock_set_t* afe_clk_config;
  

 /* Prepare the Msg to be sent to AFE */
  msg_len = sizeof( struct aprv2_packet_t ) +
            sizeof( afe_svc_cmd_set_param_t ) +
            sizeof( afe_svc_param_data_t ) +
            sizeof( afe_param_id_clock_set_t );

  csd_malloc( msg_len,( void** ) &msg );
  if( msg == NULL )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_clk_configure_i2s_clk: "
            "Failed to allocate q6msg of len=[%d]!", msg_len );
    rc = CSD_ENORESOURCE;
    return rc;
  }

  csd_memset(msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_SVC_CMD_SET_PARAM);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  /*Prepare the Message Here */
  msg_buf        = (uint8_t*)msg;
  afe_set_param  = (afe_svc_cmd_set_param_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data = (afe_svc_param_data_t*)(afe_set_param + 1);
  afe_clk_config    = (afe_param_id_clock_set_t*)(afe_param_data + 1);

  csd_memscpy(afe_clk_config, sizeof(afe_param_id_clock_set_t), clk_cfg, 
  							  sizeof(afe_param_id_clock_set_t));
  	
  afe_set_param->payload_size         = sizeof(afe_svc_param_data_t) +
                                        sizeof(afe_param_id_clock_set_t);
  afe_set_param->payload_address_lsw  = NULL;
  afe_set_param->payload_address_msw  = NULL;
  afe_set_param->mem_map_handle       = NULL;

  afe_param_data->module_id       = AFE_MODULE_CLOCK_SET;
  afe_param_data->param_id        = AFE_PARAM_ID_CLOCK_SET;
  afe_param_data->param_size      = sizeof(afe_param_id_clock_set_t);
  afe_param_data->reserved        = 0;
  
  if ( afe_clk_config->clk_id == AFE_CLOCK_SET_CLOCK_ID_MCLK_3 )
  {
	  rc = csd_q6_afe_send_command( msg,TRUE );
  }
  else
  {
	rc = csd_q6_afe_send_command( msg,TRUE );

    if( CSD_EOK == rc )
   	{
	  afe_clk_config->clk_freq_in_hz = CSD_DEV_CODEC_MCLK_FREQ;
	  afe_clk_config->clk_id = AFE_CLOCK_SET_CLOCK_ID_MCLK_3;
      rc = csd_q6_afe_send_command( msg,TRUE );
   	}
  }
  if ( rc )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_clk_configure_i2s_clk: send command to afe failed [0x%x] ", rc );
  }
  
  csd_free(msg);

 return rc;

}
/*=============================================================================

  FUNCTION      csd_dev_afe_set_lpass_shared_clock

  DESCRIPTION   This function configures the lpass core shared clock 

  DEPENDENCIES  

  PARAMETERS    @param <> :uint16_t tx_afe_id
                @param <> :afe_param_id_lpass_core_shared_clk_cfg_t *clk_cfg

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_set_lpass_shared_clock(uint16_t afe_port_id,
                              const afe_param_id_lpass_core_shared_clk_cfg_t *shared_clk_cfg)
{
  int32_t rc = CSD_EOK;
  struct q6msg q6_msg;
  struct q6msg_lpass_core_shared_clk_cfg *clk_msg;
  
  CSD_MSG( CSD_PRIO_MEDIUM,"csd_dev_afe_set_lpass_shared_clock: "
           "Config LPASS core shared clock for afe_port_id = %d", afe_port_id);
           
  csd_memset(&q6_msg, 0, sizeof(q6_msg));
  
  get_common_q6_afe_msg_hdr(&q6_msg.hdr,AFE_PORT_CMD_SET_PARAM_V2);  
  clk_msg = &(q6_msg.u.afe_lpass_core_shared_clk_cfg);
  
  clk_msg->set_param.mem_map_handle = NULL;
  clk_msg->set_param.port_id = afe_port_id;
  clk_msg->set_param.payload_size = sizeof(afe_port_param_data_v2_t) +
                                    sizeof(afe_param_id_lpass_core_shared_clk_cfg_t);

  clk_msg->param_data.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
  clk_msg->param_data.param_id = AFE_PARAM_ID_LPASS_CORE_SHARED_CLOCK_CONFIG ;
  clk_msg->param_data.param_size = sizeof(afe_param_id_lpass_core_shared_clk_cfg_t);

  csd_memscpy(&(clk_msg->lpass_core_shared_clk_cfg), sizeof(clk_msg->lpass_core_shared_clk_cfg),
              shared_clk_cfg, sizeof(afe_param_id_lpass_core_shared_clk_cfg_t));

  CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_afe_set_clock: enable[%d]",
          q6_msg.u.afe_lpass_core_shared_clk_cfg.lpass_core_shared_clk_cfg.enable );

  Q6MSG_SET_PACKET_SIZE(q6_msg.hdr.header, (Q6MSG_PACKED_HEADER_SIZE +
                        sizeof(q6_msg.u.afe_lpass_core_shared_clk_cfg)));

  rc = csd_q6_afe_send_command(&q6_msg,TRUE);
  CSD_MSG(CSD_PRIO_HIGH,"csd_dev_afe_set_lpass_shared_clock: LPASS shared clock config command sent, rc[0x%x]",rc );
  
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_

  DESCRIPTION   <>

  DEPENDENCIES  

  PARAMETERS    @param <> : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_set_dtmf_state( struct csd_dev_dtmf_ctrl * dtmf_ctrl )
{
  int32_t rc = CSD_EOK;
  struct q6msg *msg = NULL;
  uint8_t *msg_buf = NULL;
  afe_ports_cmd_dtmf_ctl_t *afe_dtmf_ctl = NULL;
  int32_t temp_list_length = dtmf_ctrl->num_devs;
  uint32_t msg_len = 0;

  /* Align port_id list to 32bits aligned to cpmply with APR packet requirment. */
  temp_list_length = (temp_list_length & 1) ? (temp_list_length + 1) : temp_list_length;

  msg_len = sizeof(struct aprv2_packet_t) +
            sizeof(afe_ports_cmd_dtmf_ctl_t) +
            sizeof(uint16_t) * (temp_list_length - 2);

  csd_malloc(msg_len,(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_set_dtmf_state: "
            "Failed to allocate q6msg of len=[%d]!",msg_len);
    rc = CSD_ENORESOURCE;
    return rc;
  }
  csd_memset(msg, 0, msg_len);

  /* Handle HW afe port loopback */
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg, AFE_PORTS_CMD_DTMF_CTL);
  msg_buf      = (uint8_t*)msg;
  afe_dtmf_ctl = (afe_ports_cmd_dtmf_ctl_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_dtmf_ctl->dtmf_duration_in_ms = dtmf_ctrl->dtmf_duration_in_ms;
  afe_dtmf_ctl->dtmf_high_freq = dtmf_ctrl->dtmf_high_freq;
  afe_dtmf_ctl->dtmf_low_freq = dtmf_ctrl->dtmf_low_freq;
  afe_dtmf_ctl->dtmf_gain = dtmf_ctrl->dtmf_gain;
  afe_dtmf_ctl->num_ports = dtmf_ctrl->num_devs;

  /* TODO: need to validate slimbus devices !!!!!! */
  /* Todo: Unni - dev_id is 32bit while port id is 16 bit, potential isse. */
  csd_memscpy((void*)&(afe_dtmf_ctl->port_ids), temp_list_length * sizeof(uint16_t),
              dtmf_ctrl->p_dev_ids, dtmf_ctrl->num_devs * sizeof(uint16_t));

  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);
  rc = csd_q6_afe_send_command(msg, TRUE);

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_afe_hdmi_multi_channel_config

  DESCRIPTION   Does only Sanity check. Actual Params will be updated and
                applied on Device Enable/Start command

  DEPENDENCIES  

  PARAMETERS    @param dev : a hdmi device record
                @param hdmi_cfg : hdmi configuration

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_hdmi_multi_channel_config( const csd_dev_record_t *dev,
                                               const csd_dev_hdmi_multi_channel_cfg_t* hdmi_cfg )
{
  int32_t rc = CSD_EOK;
  /*uint16_t index = 0;

  if(NULL == hdmi_cfg)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_hdmi_multi_channel_config: "
            "NULL Param hdmi_cfg[%p]", ((void*)hdmi_cfg));
    return CSD_EBADPARAM;
  }
  if(NULL == dev)
  {
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_hdmi_multi_channel_config: "
            "Device Record is Not Active: Skip Configuring HDMI Params to AFE/DSP");
    return CSD_EOK;
  }

  index = csd_dev_get_port_index(dev->dev_info.afe_port_id);
  if (index >= CSD_DEV_PORT_INDICES_MAX)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_hdmi_multi_channel_config: "
            "Port=[0x%x] Failed to retrieve the Port Index!!",
            dev->dev_info.afe_port_id);
    rc = CSD_ENORESOURCE;
  }

  if( (CSD_EOK == rc) &&
      (CSD_DEV_AFE_PORT_CONFIGURED == csd_dev_afe_port_info[index].port_status) )
  {
    
    rc = CSD_EBADSTATE;
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_hdmi_multi_channel_config: "
            "Configuring HDMI Port=[0x%x] in Active State is "
            "Not Supported! Err=[0x%x]!!",
            dev->dev_info.afe_port_id, rc);
  }
  else
  {
    
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_afe_hdmi_multi_channel_config: "
            "AFE_Port=[0x%x] is Not configured Yet! No Need "
            "(skip) to configre DSP!",
            dev->dev_info.afe_port_id);
  }
*/
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_configure_sb_slave_params

  DESCRIPTION   <>

  DEPENDENCIES  

  PARAMETERS    @param <> : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
/*static int32_t csd_dev_afe_configure_sb_slave_params()
{
  int32_t rc = CSD_EOK;
  struct q6msg *msg = NULL;
  uint32_t msg_len = 0;
  afe_svc_cmd_set_param_t *afe_set_param = NULL;
  afe_svc_param_data_t *afe_param_data = NULL;
  afe_cdc_slimbus_slave_cfg_param_v1_t *afe_sb_data = NULL;
  uint8_t *msg_buf = NULL;
  void *cdc_sb_data = NULL;
  uint32_t cdc_sb_size = 0;
  
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_sb_slave_params: Entering");
  
  // Get the codec slimbus slave params from codec driver 
  rc = csd_dev_codec_get_slimbus_cfg_data(&cdc_sb_data, &cdc_sb_size);
  if( (rc != CSD_EOK) || (NULL == cdc_sb_data))
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_configure_sb_slave_params: "
            "Failed to retrieve codec_sb_slava_cfg_params, "
            "rc=[0x%x]!",rc);
    return rc;
  }
  
  
  msg_len = sizeof(struct aprv2_packet_t) +
            sizeof(afe_svc_cmd_set_param_t) +
            sizeof(afe_svc_param_data_t) +
            sizeof(afe_cdc_slimbus_slave_cfg_param_v1_t);

  csd_malloc(msg_len,(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_configure_sb_slave_params: "
            "Failed to allocate q6msg of len=[%d]!",msg_len);
    rc = CSD_ENORESOURCE;
    return rc;
  }

  csd_memset(msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_SVC_CMD_SET_PARAM);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  
  msg_buf        = (uint8_t*)msg;
  afe_set_param  = (afe_svc_cmd_set_param_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data = (afe_svc_param_data_t*)(afe_set_param + 1);
  afe_sb_data    = (afe_cdc_slimbus_slave_cfg_param_v1_t*)(afe_param_data + 1);
  
  afe_set_param->payload_size         = sizeof(afe_svc_param_data_t) +
                                        sizeof(afe_cdc_slimbus_slave_cfg_param_v1_t);
  afe_set_param->payload_address_lsw  = NULL;
  afe_set_param->payload_address_msw  = NULL;
  afe_set_param->mem_map_handle       = NULL;

  afe_param_data->module_id       = AFE_MODULE_CDC_DEV_CFG;
  afe_param_data->param_id        = AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG;
  afe_param_data->param_size      = sizeof(afe_cdc_slimbus_slave_cfg_param_v1_t);
  afe_param_data->reserved        = 0;
  
  
  rc = csd_dev_codec_pack_slimbus_cfg_data(cdc_sb_data,cdc_sb_size, afe_sb_data, 
                                           sizeof(afe_cdc_slimbus_slave_cfg_param_v1_t));
  if(CSD_EOK == rc)
  {
    rc = csd_q6_afe_send_command(msg,TRUE);
  }
  
  csd_free(msg);
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_sb_slave_params: Exiting rc=[0x%x]",rc );
  
  return rc;
}*/

/*=============================================================================

  FUNCTION      csd_dev_afe_configure_cdc_aanc_version

  DESCRIPTION   Send hardware Block version being used for AFE AANC
                Definition of Hardware Block - @see adsp_afe_service_command.h

  DEPENDENCIES  @see adsp_afe_service_command.h

  PARAMETERS    @param hw_version: hardware version understanable by afe

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
/*static int32_t csd_dev_afe_configure_cdc_aanc_version(uint32 hw_version)
{
  int32_t                         rc              = CSD_EOK;
  struct q6msg                    *msg            = NULL;
  uint32_t                        msg_len         = 0;
  afe_svc_cmd_set_param_t         *afe_set_param  = NULL;
  afe_svc_param_data_t            *afe_param_data = NULL;
  afe_cdc_aanc_version_param_v1_t *afe_aanc_ver   = NULL;
  uint8_t                         *msg_buf        = NULL;

  if( (AFE_AANC_HW_BLOCK_VERSION_2 != hw_version) &&
      (AFE_AANC_HW_BLOCK_VERSION_1 != hw_version) )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_configure_cdc_aanc_version: "
            "configure_cdc_aanc_version(%lu) is not supported!!",
            hw_version);
    return CSD_EBADPARAM;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_cdc_aanc_version: Entering");

  msg_len = sizeof(struct aprv2_packet_t) +
            sizeof(afe_svc_cmd_set_param_t) +
            sizeof(afe_svc_param_data_t) +
            sizeof(afe_cdc_aanc_version_param_v1_t);

  csd_malloc(msg_len,(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_configure_cdc_aanc_version: "
            "Unable to malloc size[%d]", msg_len);
    return CSD_ENORESOURCE;
  }

  csd_memset(msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_SVC_CMD_SET_PARAM);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  
  msg_buf         = (uint8_t*)msg;
  afe_set_param   = (afe_svc_cmd_set_param_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data  = (afe_svc_param_data_t*)(afe_set_param + 1);
  afe_aanc_ver    = (afe_cdc_aanc_version_param_v1_t*)(afe_param_data + 1);

  afe_set_param->payload_size        = sizeof(afe_svc_param_data_t) +
                                       sizeof(afe_cdc_aanc_version_param_v1_t);
  afe_set_param->payload_address_lsw = NULL;
  afe_set_param->payload_address_msw = NULL;
  afe_set_param->mem_map_handle      = NULL;

  afe_param_data->module_id          = AFE_MODULE_CDC_DEV_CFG;
  afe_param_data->param_id           = AFE_PARAM_ID_CDC_AANC_VERSION;
  afe_param_data->param_size         = sizeof(afe_cdc_aanc_version_param_v1_t);
  afe_param_data->reserved           = 0;

  afe_aanc_ver->cdc_aanc_minor_version = AFE_API_VERSION_CDC_AANC_VERSION;
  afe_aanc_ver->aanc_hw_version        = hw_version;

  rc = csd_q6_afe_send_command(msg,TRUE);
  csd_free(msg);
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_cdc_aanc_version: Exiting rc=[0x%x]",rc );

  return rc;
}*/

/*=============================================================================

  FUNCTION      csd_dev_afe_configure_cdc_aanc_register_params

  DESCRIPTION   Query codec module fro codec specific data, and pass it on to
                afe module

  DEPENDENCIES

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
/*static int32_t csd_dev_afe_configure_cdc_aanc_register_params()
{
  int32_t                    rc              = CSD_EOK;
  struct q6msg               *msg            = NULL;
  uint32_t                   msg_len         = 0;
  afe_svc_cmd_set_param_t    *afe_set_param  = NULL;
  afe_svc_param_data_t       *afe_param_data = NULL;
  afe_cdc_reg_cfg_param_v1_t *afe_sb_data    = NULL;
  uint8_t                    *msg_buf        = NULL;
  void                       *codec_cfg_data = NULL;
  uint32_t                   codec_reg_count = 0;
  uint32_t                   codec_hw_ver    = 0;
  uint32_t                   index           = 0;
  uint32_t                   reg_param_size  = sizeof(afe_svc_param_data_t) +
                                               sizeof(afe_cdc_reg_cfg_param_v1_t);
  
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_cdc_aanc_register_params: Entering");
  
  
  rc = csd_dev_codec_get_aanc_reg_cfg_data(&codec_cfg_data, &codec_reg_count,
                                           &codec_hw_ver);
  if(CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_configure_cdc_aanc_register_params: "
            "Failed to retrieve codec_aanc_reg_cfg_data, "
            "rc=[0x%x]!",rc);
    return rc;
  }

  if( 0 == codec_reg_count)
  {
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_afe_configure_cdc_aanc_register_params: "
            "Zero Registers!!  in codec_aanc_reg_cfg_data, "
            "rc=[0x%x]!",rc);
    
  }
  else
  {
    msg_len = (sizeof(struct aprv2_packet_t) +
               sizeof(afe_svc_cmd_set_param_t) +
               ((codec_reg_count)*(reg_param_size)));

	csd_malloc(msg_len,(void**) &msg);
	if( msg == NULL )
	{
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_configure_cdc_aanc_register_params: "
              "Failed to allocate q6msg of len=[%d]!",msg_len);
      rc = CSD_ENORESOURCE;
    }
    else
    {
      csd_memset(msg, 0, msg_len);
      get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_SVC_CMD_SET_PARAM);
      Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

      
      msg_buf       = (uint8_t*)msg;
      afe_set_param = (afe_svc_cmd_set_param_t*)(msg_buf + sizeof(struct aprv2_packet_t));
        
      afe_set_param->payload_size         = ((codec_reg_count)*(reg_param_size));
      afe_set_param->payload_address_lsw  = NULL;
      afe_set_param->payload_address_msw  = NULL;
      afe_set_param->mem_map_handle       = NULL;

      msg_buf  = (uint8_t*)(afe_set_param + 1);
      for(index = 0;index<codec_reg_count;index++)
      {
        afe_param_data = (afe_svc_param_data_t*)(msg_buf  + ((index)*(reg_param_size)));
        afe_sb_data  = (afe_cdc_reg_cfg_param_v1_t*)(afe_param_data + 1);
                  
        afe_param_data->module_id       = AFE_MODULE_CDC_DEV_CFG;
        afe_param_data->param_id        = AFE_PARAM_ID_CDC_REG_CFG;
        afe_param_data->param_size      = sizeof(afe_cdc_reg_cfg_param_v1_t);
        afe_param_data->reserved        = 0;
        
        
        rc = csd_dev_codec_pack_aanc_cdc_reg_cfg_data(codec_cfg_data, afe_sb_data, index);
        if(CSD_EOK != rc)
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_configure_cdc_aanc_register_params: "
                  "afe_pack_adie_aanc_cdc_reg_cfg_data Failed"
                  "with Err=[0x%x]",rc );
          break;
        }
      }
      
      
      if( CSD_EOK == rc )
      {
        rc = csd_q6_afe_send_command(msg,TRUE);
      }
      csd_free(msg);

      
      if( CSD_EOK == rc)
      {
        rc = csd_dev_afe_configure_cdc_aanc_version( codec_hw_ver );
      }
    }
  }
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_cdc_aanc_register_params: "
          "Exiting rc=[%x]",rc );
  return rc;
}*/

/*=============================================================================

  FUNCTION      csd_dev_afe_init_cdc_register_config

  DESCRIPTION   Trigger afe to initialize all the prior send configurations

  DEPENDENCIES

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
/*static int32_t csd_dev_afe_init_cdc_register_config()
{
  int32_t                 rc              = CSD_EOK;
  struct q6msg           *msg             = NULL;
  uint32_t                msg_len         = 0;
  afe_svc_cmd_set_param_t *afe_set_param  = NULL;
  afe_svc_param_data_t    *afe_param_data = NULL;
  uint8_t                 *msg_buf        = NULL;

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_init_cdc_register_config: Entering");
  
  msg_len = (sizeof(struct aprv2_packet_t) +
             sizeof(afe_svc_cmd_set_param_t) +
             sizeof(afe_svc_param_data_t));
  csd_malloc(msg_len,(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_init_cdc_register_config: "
            "Unable to allocate size %d", msg_len);
    return CSD_ENORESOURCE;
  }
  csd_memset(msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_SVC_CMD_SET_PARAM);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  
  msg_buf        = (uint8_t*)msg;
  afe_set_param  = (afe_svc_cmd_set_param_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data = (afe_svc_param_data_t*)(afe_set_param + 1);
  
  afe_set_param->payload_size        = sizeof(afe_svc_param_data_t);
  afe_set_param->payload_address_lsw = NULL;
  afe_set_param->payload_address_msw = NULL;
  afe_set_param->mem_map_handle      = NULL;

  afe_param_data->module_id          = AFE_MODULE_CDC_DEV_CFG;
  afe_param_data->param_id           = AFE_PARAM_ID_CDC_REG_CFG_INIT;
  afe_param_data->param_size         = 0;
  afe_param_data->reserved           = 0;

  rc = csd_q6_afe_send_command(msg,TRUE);
  csd_free(msg);
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_init_cdc_register_config: "
          "Exiting, rc=[0x%x]",rc );
  
  return rc;
}*/

/*=============================================================================

  FUNCTION      csd_dev_afe_configure_aanc_cdc_params

  DESCRIPTION   AANC codec specific afe configuration sequence

  DEPENDENCIES

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_configure_aanc_cdc_params()
{
  int32_t rc = CSD_EOK;
  
  /* Configure the AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG */
  /*rc = csd_dev_afe_configure_sb_slave_params();
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_aanc_cdc_params: "
          "configure_sb_slave_params completed with "
          "rc=[0x%x]", rc);
   
  
  if(CSD_EOK == rc)
  {
    rc = csd_dev_afe_configure_cdc_aanc_register_params();
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_aanc_cdc_params: "
            "configure_cdc_aanc_register_params completed with "
            "rc=[0x%x]", rc);
  }
  
  
  if(CSD_EOK == rc)
  {
    rc = csd_dev_afe_init_cdc_register_config();
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_configure_aanc_cdc_params: "
            "init_cdc_register_config completed with "
            "rc=[0x%x]", rc);
  }

  
  */
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_configure_aanc_port_params

  DESCRIPTION   AANC afe port specific configuration

  DEPENDENCIES  n/a

  PARAMETERS    @param ref_dev : reference device record
                @param tx_dev  : Tx device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_configure_aanc_port_params(const csd_dev_record_t* ref_dev, 
                                               const csd_dev_record_t* tx_dev)
{
  int32_t                      rc              = CSD_EOK;
  /*struct q6msg                 *msg            = NULL;
  uint32_t                     msg_len         = 0;
  uint8_t                      *msg_buf        = NULL;
  afe_port_cmd_set_param_v2_t  *afe_set_param  = NULL;
  afe_port_param_data_v2_t     *afe_param_data = NULL;
  afe_aanc_port_cfg_param_v1_t *aanc_cfg       = NULL;
  afe_aanc_cfg1_param_v1_t     *aanc_cfg1      = NULL;
  afe_aanc_cfg2_param_v3_t     *aanc_cfg2      = NULL;
  
  uint32_t cfg1_size = sizeof(afe_aanc_cfg1_param_v1_t);
  uint32_t cfg2_size = sizeof(afe_aanc_cfg2_param_v3_t);
  
  msg_len = ( sizeof(struct aprv2_packet_t)    + sizeof(afe_port_cmd_set_param_v2_t) +
              sizeof(afe_port_param_data_v2_t) + sizeof(afe_aanc_port_cfg_param_v1_t) +
              sizeof(afe_port_param_data_v2_t) + sizeof(afe_aanc_cfg1_param_v1_t) +
              sizeof(afe_port_param_data_v2_t) + sizeof(afe_aanc_cfg2_param_v3_t));

  csd_malloc(msg_len,(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_configure_aanc_port_params: "
            "Failed to allocate q6msg of len=[%d]!",msg_len);
    return CSD_ENORESOURCE;
  }

  csd_memset(msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  
  msg_buf        = (uint8_t*)msg;
  afe_set_param  = (afe_port_cmd_set_param_v2_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data = (afe_port_param_data_v2_t*)(afe_set_param + 1);
  aanc_cfg       = (afe_aanc_port_cfg_param_v1_t*)(afe_param_data + 1);

  afe_set_param->payload_size = ( ((3)*(sizeof(afe_port_param_data_v2_t))) +
                                   sizeof(afe_aanc_port_cfg_param_v1_t) +
                                   sizeof(afe_aanc_cfg1_param_v1_t) +
                                   sizeof(afe_aanc_cfg2_param_v3_t) );
  afe_set_param->payload_address_lsw  = NULL;
  afe_set_param->payload_address_msw  = NULL;
  afe_set_param->mem_map_handle       = NULL;
  afe_set_param->port_id              = tx_dev->dev_info.afe_port_id;

  afe_param_data->module_id   = AFE_MODULE_AANC;
  afe_param_data->param_id    = AFE_PARAM_ID_AANC_PORT_CONFIG;
  afe_param_data->param_size  = sizeof(afe_aanc_port_cfg_param_v1_t);
  
  aanc_cfg->aanc_port_cfg_minor_version = AFE_API_VERSION_AANC_PORT_CONFIG;
  aanc_cfg->tx_port_sample_rate    = (uint32_t)tx_dev->curr_attrib.sample_rate;
  aanc_cfg->tx_port_channel_map[0] = AANC_TX_VOICE_MIC;
  aanc_cfg->tx_port_channel_map[1] = AANC_TX_NOISE_MIC;
  aanc_cfg->tx_port_channel_map[2] = AANC_TX_ERROR_MIC;
  aanc_cfg->tx_port_channel_map[3] = AANC_TX_MIC_UNUSED;
  aanc_cfg->tx_port_num_channels   = tx_dev->dev_info.channel_config;
  aanc_cfg->rx_path_ref_port_id    = (uint16_t)ref_dev->dev_info.afe_port_id;
  aanc_cfg->ref_port_sample_rate   = (uint32_t)ref_dev->curr_attrib.sample_rate;

  afe_param_data             = (afe_port_param_data_v2_t*)(aanc_cfg + 1);
  afe_param_data->module_id  = AFE_MODULE_AANC;
  afe_param_data->param_id   = AFE_PARAM_ID_AANC_CFG1;
  afe_param_data->param_size = sizeof(afe_aanc_cfg1_param_v1_t);
  
  aanc_cfg1      = (afe_aanc_cfg1_param_v1_t*)(afe_param_data + 1);
  afe_param_data = (afe_port_param_data_v2_t*)(aanc_cfg1 + 1);
  aanc_cfg2      = (afe_aanc_cfg2_param_v3_t*)(afe_param_data + 1);
  
  afe_param_data->module_id  = AFE_MODULE_AANC;
  afe_param_data->param_id   = AFE_PARAM_ID_AANC_CFG2;
  afe_param_data->param_size = sizeof(afe_aanc_cfg2_param_v3_t);
  
  
  rc = csd_acdb_get_afe_aanc_cfg(tx_dev->dev_id, aanc_cfg1, &cfg1_size,
                                 aanc_cfg2, &cfg2_size);
  CSD_MSG(CSD_PRIO_LOW,"csd_dev_afe_configure_aanc_port_params: "
          "Retrieve AANC CFG Params for tx_dev_id=[%d] from ACDB "
          "completed with rc=[0x%x]", tx_dev->dev_id, rc );
  if(CSD_EOK == rc)
  {
    afe_param_data->param_size = cfg2_size;  //added to support V2/V3 difference
    rc = csd_q6_afe_send_command(msg, TRUE);
  }
  csd_free(msg);
  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_configure_aanc_port_params: "
            "AANC CONFIGURE PORT PARAMS Failed for tx_dev_id="
            "[%d] with Err=[0x%x]", tx_dev->dev_id, rc);
  }
  */
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_aanc_control

  DESCRIPTION   Call to SET the AANC Enable Parameter TO AFE

  DEPENDENCIES  n/a

  PARAMETERS    @param tx_afe_id : tx afe port id associated with aanc
                @param aanc_enable_mode  : flag to enable/disable AANC

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_aanc_control(uint32_t tx_afe_id, bool_t aanc_enable_mode)
{
  int32_t                     rc              = CSD_EOK;
  /*struct q6msg                *msg            = NULL;
  uint8_t                     *msg_buf        = NULL;
  uint32_t                    msg_len         = 0;
  afe_port_cmd_set_param_v2_t *afe_set_param  = NULL;
  afe_port_param_data_v2_t    *afe_param_data = NULL;
  afe_mod_enable_param_t      *aanc_enable    = NULL;

  msg_len = ( sizeof(struct aprv2_packet_t) +
              sizeof(afe_port_cmd_set_param_v2_t) +
              sizeof(afe_port_param_data_v2_t) +
              sizeof(afe_mod_enable_param_t));

  csd_malloc(msg_len,(void**) &msg);
  if( msg == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_afe_aanc_control: "
            "Failed to allocate memory size of (%d)!",
            msg_len);
    return CSD_ENORESOURCE;
  }
  
  csd_memset(msg, 0, msg_len);
  get_common_q6_afe_msg_hdr((struct aprv2_packet_t*)msg,AFE_PORT_CMD_SET_PARAM_V2);
  Q6MSG_SET_PACKET_SIZE(msg->hdr.header, msg_len);

  
  msg_buf                 = (uint8_t*)msg;
  afe_set_param           = (afe_port_cmd_set_param_v2_t*)(msg_buf + sizeof(struct aprv2_packet_t));
  afe_param_data          = (afe_port_param_data_v2_t*)(afe_set_param + 1);
  aanc_enable             = (afe_mod_enable_param_t*)(afe_param_data + 1);

  afe_set_param->payload_size        = sizeof(afe_port_param_data_v2_t) +
                                       sizeof(afe_mod_enable_param_t);
  afe_set_param->payload_address_lsw = NULL;
  afe_set_param->payload_address_msw = NULL;
  afe_set_param->mem_map_handle      = NULL;
  afe_set_param->port_id             = tx_afe_id;

  afe_param_data->module_id  = AFE_MODULE_AANC;
  afe_param_data->param_id   = AFE_PARAM_ID_ENABLE;
  afe_param_data->param_size = sizeof(afe_mod_enable_param_t);

  aanc_enable->enable   = (uint16_t)aanc_enable_mode;
  aanc_enable->reserved = 0;

  rc = csd_q6_afe_send_command(msg, TRUE);
  csd_free(msg);
  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_aanc_control: "
            "AANC ENABLE command Failed for mode=[0x%x] with "
            "Err=[0x%x]", tx_afe_id, rc);
  }*/
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_aanc_configure

  DESCRIPTION   Call to configure the AANC Parameters to AFE:
                [Params from both ACDB and ADIE]

  DEPENDENCIES  n/a

  PARAMETERS    @param ref_dev : reference device record
                @param tx_dev  : Tx device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_aanc_configure(const csd_dev_record_t* ref_dev,
                                   const csd_dev_record_t* tx_dev)
{
  int32_t rc = CSD_EOK;
  
  /* Apply codec related AFE Service Params :: Port Independent  (one time)::
   * Can be called during INIT */
  /* TODO: Unni - try out a static flag to avoid this configuring afe multiple
   * times */
  //rc = csd_dev_afe_configure_aanc_cdc_params();
  
  /* Apply AANC related config data to AFE */
  /*if( CSD_EOK == rc )
  {
    rc = csd_dev_afe_configure_aanc_port_params(ref_dev, tx_dev);
  }
  */
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_aanc_configure

  DESCRIPTION   Call to get the AANC Enable Parameter from AFE: via ACDB

  DEPENDENCIES  n/a

  PARAMETERS    @param tx_dev_id : tx device id
                @param p_enable  : reference for returning aanc flag

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_afe_get_aanc_enable_mode(uint32_t tx_dev_id, bool_t *p_enable)
{
  int32_t                rc           = CSD_EOK;
  /*afe_mod_enable_param_t aanc_enable;
  uint32_t               param_size   = sizeof(afe_mod_enable_param_t);
  
  csd_memset(&aanc_enable, 0, sizeof(aanc_enable));
  
  if(NULL != p_enable)
  {
    
    rc = csd_acdb_get_afe_aanc_enable(tx_dev_id, &aanc_enable, &param_size);
    *p_enable = (bool_t)aanc_enable.enable;
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_afe_get_aanc_enable_mode: "
            "get_aanc_enable_mode completed with rc=[0x%x], "
            "aanc_mode=[%d]",
            rc, aanc_enable.enable);
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_afe_get_aanc_enable_mode: "
            "Invalid Param enable[%p]",((void*)p_enable));
  }*/
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_restart

  DESCRIPTION   <> Used to switch a sampling rate dynamically on a I2S device

  DEPENDENCIES  none

  PARAMETERS    @param : uint32_t rx_afe_id
                @param : uint32_t tx_afe_id
                @param : uint32_t sample_rate

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_restart(uint16_t rx_port_id,
                            uint16_t tx_port_id,
                            uint32_t sample_rate )
{
  int32_t rc = CSD_EOK;
  struct q6msg q6_msg;

  csd_memset(&q6_msg, 0, sizeof(q6_msg));
  get_common_q6_afe_msg_hdr(&q6_msg.hdr, AFE_PORT_CMD_DEVICES_RESTART);
  q6_msg.u.afe_port_restart.rx_port_id = rx_port_id;
  q6_msg.u.afe_port_restart.tx_port_id = tx_port_id;
  q6_msg.u.afe_port_restart.sample_rate = sample_rate;
  Q6MSG_SET_PACKET_SIZE(q6_msg.hdr.header,
                (Q6MSG_PACKED_HEADER_SIZE+ sizeof(q6_msg.u.afe_port_restart)));

  rc = csd_q6_afe_send_command(&q6_msg, TRUE);

  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_deinit

  DESCRIPTION   Clear csd_dev_afe_port_info

  DEPENDENCIES

  PARAMETERS   n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_deinit(void)
{
  csd_memset(csd_dev_afe_port_info, 0, sizeof(csd_dev_afe_port_info));
  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_afe_init

  DESCRIPTION   Clear csd_dev_afe_port_info

  DEPENDENCIES

  PARAMETERS   n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_afe_init( void )
{
  csd_memset(csd_dev_afe_port_info, 0, sizeof(csd_dev_afe_port_info));
  return CSD_EOK;
}
