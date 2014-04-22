/*==============================================================================
  @file csd_dev_tgt.c

  The objective of this source file is to push down the CSD_DEV_IOCTLs to the
  corresponding sub-components.

  Copyright (c) 2008-13, 2016 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/src/csd_dev_tgt.c#2 $
  $DateTime: 2019/01/03 07:23:13 $
  $Author: pwbldsvc $
  $Change: 17964421 $
  $Revision: #2 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/15/16   rk      Abstracting OSAL layer
  09/01/14   rp      enhanced the csd init debug buffer
  02/12/14   jk      Added support for CSD_DEV_CMD_RESTART 
  02/20/13   suns    Dev modules unification
  01/10/13   rp      Added SideTone IIR filter support
  07/13/12   rp      Added AFE support.
  05/01/12   rp      Modified code to enable tx dtmf generation.
  01/17/12   jp      Support for DTMF thru AFE
  04/27/010  suns    Initial Version
==============================================================================*/
#include "csd_dev_afe.h"
#include "csd_dev_acdb.h"
#include "csd_dev_codec.h"
#include "csd_dev_clk.h"
#include "csd_dev_tlmm.h"
#include "csd_dev_oem_plugin_intf.h"
#include "DALSys.h"
#include "csd_debug_msg.h"
#include "csd_dev_debug.h"
#include "csd_dev_common.h"
#include "csd_os_dependencies.h"
#include "csd_debug_info.h"
#include "csd_dev_mbhc.h"

/*=============================================================================

  FUNCTION      csd_dev_control_device_afe_loopback

  DESCRIPTION   This function enable/disable AFE LOOPBACK on devices

  DEPENDENCIES  n/a

  PARAMETERS    @param
                rx_dev : input RX device
                rx_dev : input RX device
                loopback: loopback control data

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static uint32_t csd_dev_control_device_afe_loopback ( const csd_dev_record_t* rx_dev,
                                                      const csd_dev_record_t* tx_dev,
                                                      struct csd_dev_afe_loopback *loopback )
{
  uint32_t rc = CSD_EOK;  
  uint16_t rx_afeportid = 0;
  uint16_t tx_afeportid = 0;
  if( (NULL != rx_dev) && (NULL != tx_dev) )
  {
    rx_afeportid = rx_dev->dev_info.afe_port_id;
    tx_afeportid = tx_dev->dev_info.afe_port_id;
    rc = csd_dev_afe_enable_loopback( rx_afeportid, tx_afeportid,
                                    loopback->enable, loopback->afe_mode );
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_control_device_afe_loopback: "
            "Bad param rx[%p] tx[%p]", ((void*)rx_dev), ((void*)tx_dev));
    rc = CSD_EBADPARAM;
  }
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_control_device_dtmf

  DESCRIPTION   This function enable/disable DTMF on devices

  DEPENDENCIES  n/a

  PARAMETERS    @param dtmf_ctrl : input dtmf control data

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static uint32_t csd_dev_control_device_dtmf(struct csd_dev_dtmf_ctrl *dtmf_ctrl)
{
  uint32_t rc = CSD_EOK;
  csd_dev_record_t* a_dev = NULL;
  uint32_t dev_id = CSD_DEV_ID_INVALID;
  const uint16_t afe_id_max = ( AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE +
                                AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE );
  uint16_t afe_ids[afe_id_max];
  int32_t i, j;
  uint16_t num_afe_ports = 0;
  uint16_t temp_afe_id;
  struct csd_dev_dtmf_ctrl afe_dtmf_ctrl;

  
  csd_memset( afe_ids, AFE_PORT_ID_INVALID, sizeof(uint16_t)*afe_id_max);


  for (i = 0; i < dtmf_ctrl->num_devs ; i++)
  {
    if(num_afe_ports >= afe_id_max)
    {
      /* requested number of afe ports more that available afe ports */
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_control_device_dtmf: "
              "afe port id overflow in DTMF command!");
      rc = CSD_EUNEXPECTED;
      break;
    }
    dev_id = dtmf_ctrl->p_dev_ids[i];
    a_dev = csd_dev_get_active_record(dev_id);

    if( a_dev )
    {
      temp_afe_id = (uint16_t) a_dev->dev_info.afe_port_id;

      /*Only add into afe_ids list when a new afe_port found from dev_id lists*/
      for (j = 0; j < num_afe_ports ; j++)
      {
        if (afe_ids[j] == temp_afe_id)
        {
          break;
        }
      }
      if( j == num_afe_ports )
      {
        afe_ids[num_afe_ports] = temp_afe_id;
        num_afe_ports++;
      }
    }
    else
    {
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_control_device_dtmf: "
          "Devices requested for DTMF opertions are not enabled yet!");
      rc = CSD_EBADPARAM;
      break;
    }
  }
  if ( (rc == CSD_EOK) && (num_afe_ports > 0) )
  {
    csd_memscpy(&afe_dtmf_ctrl, sizeof(afe_dtmf_ctrl),
                dtmf_ctrl, sizeof(struct csd_dev_dtmf_ctrl));
    afe_dtmf_ctrl.num_devs = num_afe_ports;
    afe_dtmf_ctrl.p_dev_ids = (uint32_t *)&afe_ids[0];  
    rc = csd_dev_afe_set_dtmf_state(&afe_dtmf_ctrl);
  }
  
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_control_device_ANC

  DESCRIPTION   This function enable/disable Active Noise Cancellation on a rx device.
                Paired ANC Tx device is used to set the ADIE_CODEC_ANC Tx Path profile.

  DEPENDENCIES  n/a

  PARAMETERS    @param rx_dev : input RX device
                       Tx_dev : input TX device
                       enable_flag: enable/disable flag.

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static uint32_t csd_dev_control_device_ANC( const csd_dev_record_t* rx_dev,
                                            const csd_dev_record_t* tx_dev,
                                            bool_t enable_flag )
{
  uint32_t rc = CSD_EOK;
  
  switch(enable_flag)
  {
    case CSD_DEV_CODEC_ANC_DISABLE:
    case CSD_DEV_CODEC_ANC_ENABLE:
      rc = csd_dev_codec_adie_anc_control(rx_dev, tx_dev, enable_flag );
      break;
    default:
      rc = CSD_EUNSUPPORTED;
      break;
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_control_device_sidetone

  DESCRIPTION   This function enable or disable the AFE side tone
                if side tone is enabled, then this function also configures and
                enables the Side tone IIR filter.

  DEPENDENCIES  

  PARAMETERS    @param <> : csd_dev_record_t* rx_dev
                @param  <> : csd_dev_record_t* tx_dev
                            csd_dev_sidetone_ctrl* sidetone
  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static uint32_t csd_dev_control_device_sidetone( const csd_dev_record_t* rx_dev,
                                                 const csd_dev_record_t* tx_dev,
                                                 csd_dev_sidetone_ctrl_t* sidetone )
{
  uint32_t rc = CSD_EOK;
  bool_t sidetone_enable = FALSE;
  bool_t sidetone_iir_present = FALSE;
  uint16_t sidetone_gain = 0;
  uint32_t rx_afe_id = 0;
  uint32_t tx_afe_id = 0;

  if ( NULL == sidetone || NULL == rx_dev || NULL == tx_dev)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_control_device_sidetone: "
            "Bad param side[%p],rx[%p],tx[%p]!", 
            sidetone, ((void*)rx_dev), ((void*)tx_dev));
    rc = CSD_EBADPARAM;
    return rc;
  }

  rx_afe_id = rx_dev->dev_info.afe_port_id;
  tx_afe_id = tx_dev->dev_info.afe_port_id;

  /* Try Codec sidetone first*/
  rc = csd_dev_codec_sidetone_ctrl(rx_dev, tx_dev, sidetone);
  if ( CSD_EOK != rc && CSD_EUNSUPPORTED != rc )
  {
    CSD_MSG(CSD_PRIO_HIGH, "csd_dev_control_device_sidetone: "
            "codec sidetone ctrl failed with err [%d]", rc);
  }
  else if ( CSD_EUNSUPPORTED == rc )
  {
    /* Codec sidetone not supported, try AFE */
    rc = CSD_EOK;

    CSD_MSG(CSD_PRIO_HIGH, "csd_dev_control_device_sidetone: "
            "codec sidetone not supported, trying afe");

    if ( CSD_DEV_SIDETONE_DISABLE != sidetone->ctrl )
    {
      /*
       * Get gain value for sidetone enable usecase
       * Get default sidetone enable flag, will override if
       * sidetone->ctrl flag forces sidetone enable usecase
       */
      rc = csd_dev_acdb_get_afe_sidetone(sidetone->rx_dev_id, sidetone->tx_dev_id,
                                         &sidetone_enable, &sidetone_gain);
      CSD_MSG(CSD_PRIO_HIGH, "csd_dev_control_device_sidetone: "
              "AFE side tone gain values from ACDB  Enable:0x%x "
              "Side tone gain:0x%x", sidetone_enable, sidetone_enable);
    }

    if ( CSD_EOK == rc )
    {
      /*
       * Get the ACDB Side tone IIR filter enable data
       * This info is used for all cases to understand presence of iir filter
       */
      csd_dev_acdb_get_afe_sidetone_iir_enable(sidetone->rx_dev_id,
                                               sidetone->tx_dev_id,
                                               &sidetone_iir_present);
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_control_device_sidetone: "
              "Side tone IIR Enable flag :0x%x",
              sidetone_iir_present);
    }

    if ( CSD_EOK == rc )
    {
      switch ( sidetone->ctrl )
      {
        case CSD_DEV_SIDETONE_ENABLE:
          sidetone_enable = TRUE;
          /* No break, same as CSD_DEV_SIDETONE_DEFAULT with enable = TRUE */
        case CSD_DEV_SIDETONE_DEFAULT:
        {
          if ( sidetone_enable )
          {
            /*if side tone IIR is enabled then get the IIR filter data
             from ACDB and send to afe*/
            if ( sidetone_iir_present )
            {
              /*Get the side tone IIR filter data from the ACDB*/
              rc = csd_dev_afe_send_sidetone_iir_data(sidetone->rx_dev_id,
                                                      sidetone->tx_dev_id,
                                                      tx_afe_id);
            }
            if ( (CSD_EOK == rc) && (sidetone_iir_present) )
            {
              /*Send the iir enable to AFE */
              rc = csd_dev_afe_enable_sidetone_iir(tx_afe_id, TRUE);
            }
            if ( CSD_EOK == rc )
            {
              /*Send the side tone gain and enable info to AFE */
              rc = csd_dev_afe_enable_sidetone(rx_afe_id, tx_afe_id, TRUE,
                                               sidetone_gain);
            }
          }
          CSD_MSG(CSD_PRIO_HIGH, "csd_dev_control_device_sidetone:"
                  "AFE CSD_DEV_SIDETONE_DEFAULT status :0x%x", rc);
        }
          break;
        case CSD_DEV_SIDETONE_DISABLE:
        {
          /*Send the Side tone IIR disable to AFE */
          if ( sidetone_iir_present )
          {
            rc = csd_dev_afe_enable_sidetone_iir(tx_afe_id, FALSE);
            CSD_MSG(CSD_PRIO_HIGH, "csd_dev_control_device_sidetone: "
                    "Side tone IIR disabled sent to AFE  status :0x%x", rc);
          }

          /*Send the Side tone disable to AFE */
          rc = csd_dev_afe_enable_sidetone(rx_afe_id, tx_afe_id, FALSE,
                                           sidetone_gain);
          CSD_MSG(CSD_PRIO_HIGH, "csd_dev_control_device_sidetone: "
                  "AFE CSD_DEV_SIDETONE_DISABLE status :0x%x", rc);

        }
          break;
        default:
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_control_device_sidetone: "
                  "Unsupported options [%d]!", sidetone->ctrl);
          rc = CSD_EBADPARAM;
        }
          break;
      }
    }
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_control_device_sidetone: Exiting with error %x", 
          rc);
  return rc; 
}

/*=============================================================================

  FUNCTION      csd_dev_control_aanc

  DESCRIPTION   This function is used to enable or disable the AANC feature in
                both AFE and Codec.

  DEPENDENCIES  

  PARAMETERS    @param <> : csd_dev_record_t* rx_dev
                @param  <> : csd_dev_record_t* tx_dev
                            csd_cmd_dev_aanc_t* aanc_ctrl
  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
//static uint32_t csd_dev_control_aanc(const csd_dev_record_t* rx_dev, 
//                                     const csd_dev_record_t* tx_dev,
//                                     const csd_dev_record_t* ref_dev,
//                                     csd_cmd_dev_aanc_t* aanc_ctrl)
//{
 // uint32_t rc = CSD_EOK;
  /*if(NULL == aanc_ctrl || NULL == tx_dev)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_control_aanc: "
            "Null parameter for CSD_DEV_CMD_AANC_CONTROL! "
            "aanc_ctl[0x%p], tx_dev[0x%p]", ((void*)aanc_ctrl), tx_dev);
    return CSD_EBADPARAM;
  }
  else
  {
    bool_t aanc_enable_mode =  FALSE;
    
    switch (aanc_ctrl->aanc_ctrl)
    {
      case CSD_DEV_AFE_AANC_DEFAULT:
        rc = csd_dev_afe_get_aanc_enable_mode(tx_dev->dev_id, &aanc_enable_mode);

        // Update default value for above layer's record management 
        //[Unni] This can be avoided if upper layer directly contacts ACDB
         // to get the flag instead of going via afe module */
       /* aanc_ctrl->aanc_ctrl = aanc_enable_mode ? CSD_DEV_AFE_AANC_ENABLE:
                                                  CSD_DEV_AFE_AANC_DISABLE;
        break;
      case CSD_DEV_AFE_AANC_ENABLE:
        aanc_enable_mode = TRUE;
        break;
      case CSD_DEV_AFE_AANC_DISABLE:
        aanc_enable_mode = FALSE;
        break;
      default:
        rc = CSD_EUNSUPPORTED;
        break;
    }
    
    // Configure the AFE AANC PARAMS 
    //if( (CSD_EOK == rc) && (TRUE == aanc_enable_mode) && (NULL != ref_dev))
    {
      rc = csd_dev_afe_aanc_configure(ref_dev, tx_dev);
    }
    
    if((CSD_EOK == rc) && (NULL != rx_dev))
    {
      
      rc = csd_dev_codec_adie_anc_control(rx_dev, tx_dev, aanc_enable_mode);
    }
    
    if( ((CSD_EOK == rc) || (FALSE == aanc_enable_mode)) && (NULL != tx_dev) )
    {
      
      rc = csd_dev_afe_aanc_control(tx_dev->dev_info.afe_port_id, aanc_enable_mode);
      
      
      if( (CSD_EOK != rc) && (TRUE == aanc_enable_mode) && (NULL != rx_dev) )
      {
        int32_t ret = CSD_EOK;
        ret = csd_dev_codec_adie_anc_control(rx_dev, tx_dev, FALSE);
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_control_aanc: "
                "CSD_DEV_CMD_AANC_CONTROL:: AFE setup failed with Err=[0x%x]" \
                "Revert Codec AANC completed with Err=[0x%x]!", rc, ret);
      }
    }
  }*/
  //return rc;
//}

/*=============================================================================

  FUNCTION      csd_dev_tgt_ioctl

  DESCRIPTION   <>

  DEPENDENCIES  

  PARAMETERS    @param <> : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
uint32_t csd_dev_tgt_ioctl( const csd_dev_record_t* rx_dev,
                            const csd_dev_record_t* tx_dev,
                            const csd_dev_record_t* ref_dev,
                            uint32_t cmd_code, void *payload, uint32_t len )
{
  /* By default, rc is BADPARAM. If correct function is called, rc is updated */
  uint32_t rc = CSD_EBADPARAM;
  
  switch(cmd_code) 
  {
    case CSD_DEV_CMD_AFE_LOOPBACK:
    {
      if( NULL != payload && len == sizeof(csd_dev_afe_loopback_t) )
      {
        csd_dev_afe_loopback_t *loopback =( csd_dev_afe_loopback_t *) payload;
        rc = csd_dev_control_device_afe_loopback(rx_dev, tx_dev, loopback);
      }
      break;
    }
    case CSD_DEV_CMD_ANC_CONTROL:
    {
      if( (NULL != payload) && (len == sizeof(csd_cmd_dev_anc_t)) )
      {
        csd_cmd_dev_anc_t *anc_ctl = (csd_cmd_dev_anc_t*) payload;
        rc = csd_dev_control_device_ANC(rx_dev, tx_dev, anc_ctl->enable);
      }
      break;
    }
    case CSD_DEV_CMD_COMPANDING_CONTROL:
    {
      if( NULL != payload && len == sizeof(csd_dev_companding_t) )
      {
        /*suns: return unsupported for now till companding feature is integrated from ADIE
         csd_dev_companding_t *cmp_ctl = (csd_dev_companding_t*) payload;
         rc = csd_tgt_codec_adie_set_companding_state(rx_dev,cmp_ctl->comp_options);
         */
        rc = CSD_EUNSUPPORTED;
      }
      break;
    }
    case CSD_DEV_CMD_DTMF_CONTROL:
    {
      if( NULL != payload && len == sizeof(csd_dev_dtmf_ctrl_t) )
      {
        csd_dev_dtmf_ctrl_t *dtmf_ctrl = (csd_dev_dtmf_ctrl_t *) payload;
        rc = csd_dev_control_device_dtmf(dtmf_ctrl);
      }
      break;
    }
    case CSD_DEV_CMD_SIDETONE_CONTROL:
    {
      if( NULL != payload && len == sizeof(struct csd_dev_sidetone_ctrl) )
      {
        struct csd_dev_sidetone_ctrl* sidetone_ctrl =
            (struct csd_dev_sidetone_ctrl*) payload;

        rc = csd_dev_control_device_sidetone(rx_dev, tx_dev, sidetone_ctrl);
      }
      break;
    }
    /*case CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG:
    {
      if( NULL != payload
          && len == sizeof(struct csd_dev_hdmi_multi_channel_cfg) )
      {
        csd_dev_hdmi_multi_channel_cfg_t* hdmi_cfg =
            (csd_dev_hdmi_multi_channel_cfg_t*) payload;
        rc = csd_dev_afe_hdmi_multi_channel_config(rx_dev, hdmi_cfg);
      }
      break;
    }
    case CSD_DEV_CMD_AANC_CONTROL:
    {
      if(NULL != payload && len == sizeof(csd_cmd_dev_aanc_t))
      {
        csd_cmd_dev_aanc_t *aanc_ctrl = (csd_cmd_dev_aanc_t*)payload;
        rc = csd_dev_control_aanc(rx_dev, tx_dev, ref_dev, aanc_ctrl);
      }
      break;
    }
    case CSD_DEV_CMD_CONFIG_MBHC_PARAMS:
    {
      struct csd_dev_mbhc_config* dev_mbhc_ctrl = (struct csd_dev_mbhc_config*) payload;
      rc = csd_dev_control_device_mbhc(dev_mbhc_ctrl);
      break;
    }*/
    case CSD_DEV_CMD_RESTART:
    {
      csd_dev_record_t* new_rx_dev = NULL;
      csd_dev_record_t* new_tx_dev = NULL;
      uint32_t rc_tx_disable;
      uint32_t rc_rx_disable;

      if( ( NULL != payload ) && ( len == sizeof(csd_cmd_dev_restart_t) ) )
      {
        csd_cmd_dev_restart_t *restart_cmd =( csd_cmd_dev_restart_t *) payload;
        
        /* disable rx and tx device on codec*/
        rc_rx_disable = csd_dev_codec_disable(rx_dev);
        if( rc_rx_disable )
        {
           CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_tgt_ioctl: Failed to disable codec rx device %d!",rx_dev->dev_id );
           rc = CSD_EFAILED;
        }

        rc_tx_disable = csd_dev_codec_disable(tx_dev);
        if( rc_tx_disable )
        {
           CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_tgt_ioctl: Failed to disable codec tx device %d!",tx_dev->dev_id );
            rc = CSD_EFAILED;
        }

        if( ( rc_rx_disable == CSD_EOK ) && ( rc_tx_disable == CSD_EOK ) )
        {
        rc = csd_dev_afe_restart(rx_dev->dev_info.afe_port_id, tx_dev->dev_info.afe_port_id,
                                      restart_cmd->sample_rate );
        }

        new_tx_dev = csd_dev_get_active_record(restart_cmd->tx_dev_id);
        new_rx_dev = csd_dev_get_active_record(restart_cmd->rx_dev_id);
        if( rc == CSD_EOK )
        { 
        /* re-enable codec with new sampling rate*/
        new_rx_dev->curr_attrib.sample_rate = restart_cmd->sample_rate;
        new_tx_dev->curr_attrib.sample_rate = restart_cmd->sample_rate;
        }
        /* if afe failed reinitalize codec at old sampling rate*/
        else
        {
          CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_tgt_ioctl: Cannot restart afe restart failed returning to pervious sample rate!");
          rc = CSD_EFAILED;

        }
        if ( csd_dev_codec_enable(new_rx_dev) != CSD_EOK )
        {
           CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_tgt_ioctl: Failed to re-enable codec on rx dev %d!", new_rx_dev->dev_id);
           rc = CSD_EFAILED;
        }
        if ( csd_dev_codec_enable(new_tx_dev) != CSD_EOK )
        {
           CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_tgt_ioctl: Failed to re-enable codec on tx dev %d!", new_tx_dev->dev_id);
           rc = CSD_EFAILED;
        }
          
      }
      else
      {
          CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_tgt_ioctl: Cannot restart invalid payload!");
          rc = CSD_EFAILED;
      }
      break;
    }
    default:
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_tgt_ioctl: "
              "Un-suported IOCTL 0x%x for DEV management",
              cmd_code);
      rc = CSD_EUNSUPPORTED;
      break;
    }
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_debug_fill_dev_info_v2

  DESCRIPTION   This is called from the debug to fill in the csd_debug_active_dev_info_query_t
                based on csd_dev_record

  DEPENDENCIES  

  PARAMETERS    @param <> : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h, now only return CSD_EOK

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tgt_debug_fill_dev_info_v2(const csd_dev_record_t* a_dev,
                                           csd_debug_active_dev_info_query_v2_t* dev_info_rsp)
{
  uint32_t i;

  /* Going thru the device list to pick up the info */
  for(i = 0; i < dev_info_rsp->num_entries; i++)
  {
    void *wcd_user_handle;

    if(a_dev) 
    {
      dev_info_rsp->entries[i].dev_id = a_dev->dev_id;
      dev_info_rsp->entries[i].sample_rate = a_dev->curr_attrib.sample_rate;
      dev_info_rsp->entries[i].bits_per_sample = a_dev->curr_attrib.bits_per_sample;
      dev_info_rsp->entries[i].port_id = a_dev->dev_info.afe_port_id;

      wcd_user_handle = NULL;
      if(CSD_EOK == csd_dev_codec_get_adie_usage_handle(a_dev, &wcd_user_handle))
      {
        dev_info_rsp->entries[i].adie_handle = (uint32_t) (wcd_user_handle);
      }
      else
      {
        dev_info_rsp->entries[i].adie_handle = 0;
      }

      /*
      dev_info_rsp->entries[i].is_virtual_dev = a_dev->dev_info.ulIsVirtualDevice;
      */
      if( ACDB_DEVICE_TYPE_VIRTUAL_PARAM == a_dev->dev_info.interface_type )
      {
        dev_info_rsp->entries[i].timing_mode = a_dev->dev_info.u.virtual_dev_info.afe_timing_mode;
        dev_info_rsp->entries[i].is_virtual_dev = TRUE;
      }
      else
      {
        dev_info_rsp->entries[i].is_virtual_dev = FALSE;
        /*timing mode is only used on virtial devices */
        dev_info_rsp->entries[i].timing_mode = 0;
      }
      a_dev = a_dev->next;
    }
    else
    {
      break;
    }
  }
  dev_info_rsp->num_entries = i;
  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_debug_fill_dev_info

  DESCRIPTION   This is called from the debug to fill in the csd_debug_active_dev_info_query_t
                based on csd_dev_record

  DEPENDENCIES  

  PARAMETERS    @param <> : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h, now only return CSD_EOK

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tgt_debug_fill_dev_info(const csd_dev_record_t* a_dev,
                                        csd_debug_active_dev_info_query_t* dev_info_rsp)
{
  uint32_t i;

  for(i = 0; i < dev_info_rsp->num_entries; i++)
  {
    if(a_dev) 
    {
      dev_info_rsp->entries[i].dev_id = a_dev->dev_id;
      dev_info_rsp->entries[i].sample_rate = a_dev->curr_attrib.sample_rate;
      dev_info_rsp->entries[i].bits_per_sample = a_dev->curr_attrib.bits_per_sample;
      dev_info_rsp->entries[i].port_id = a_dev->dev_info.afe_port_id;
      /*
      dev_info_rsp->entries[i].is_virtual_dev = a_dev->dev_info.ulIsVirtualDevice;
      */
      if( a_dev->dev_info.interface_type == ACDB_DEVICE_TYPE_VIRTUAL_PARAM )
      {
        dev_info_rsp->entries[i].timing_mode = a_dev->dev_info.u.virtual_dev_info.afe_timing_mode;
        dev_info_rsp->entries[i].is_virtual_dev = TRUE;
      }
      else
      {
        dev_info_rsp->entries[i].is_virtual_dev = FALSE;
        /*timing mode is only used on virtial devices */
        dev_info_rsp->entries[i].timing_mode = 0;
      }
      a_dev = a_dev->next;
    }
    else
    {
      break;
    }
  }
  dev_info_rsp->num_entries = i;
  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_init

  DESCRIPTION   Initializing sequence for device target specific module.
                 - Failure in any module will not allow sequence to continue
                 - Status of each module is reflected on csd_debug_init_info

  DEPENDENCIES  n/a

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tgt_init(void)
{
  int32_t rc = CSD_EOK;

  rc = csd_dev_oem_init();
  CSD_MSG(CSD_PRIO_LOW,"csd_dev_tgt_init: "
          "csd_dev_oem_init returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_OEM_INIT,0); 

  if (rc == CSD_EOK)
  {
    rc = csd_dev_tlmm_init();
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_init: "
            "Tlmm init returned with rc = 0x%x", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                       //  CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,0); 
  }

  if (rc == CSD_EOK)
  {
    rc = csd_dev_clk_init();
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_init: "
            "Clk init returned with rc = 0x%x", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CLK_INIT,0); 
  }

  if (rc == CSD_EOK)
  {
    rc = csd_dev_codec_init();
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_init: "
            "Codec init returned with rc = 0x%x", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,0); 
  }

  if (rc == CSD_EOK)
  {
    rc = csd_dev_afe_init();
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_init: "
            "Afe init returned with rc = 0x%x", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_AFE_INIT,0); 
  }

  if (rc == CSD_EOK)
  {
    rc = csd_dev_mbhc_init();
    CSD_MSG(CSD_PRIO_LOW,"csd_dev_tgt_init: "
            "mbhc init returned with rc = 0x%x", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_MBHC_INIT,0); 
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_deinit

  DESCRIPTION   De-initialize all the device target specific module.
                 - Failure in any module de-init will not hinder the sequence
                 - Status of each module is reflected on csd_debug_init_info

  DEPENDENCIES  n/a

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tgt_deinit(void)
{
  int32_t rc = CSD_EOK;

  rc = csd_dev_mbhc_deinit();
  CSD_MSG(CSD_PRIO_LOW,"csd_dev_tgt_deinit: "
          "mbhc deinit returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_MBHC_INIT,0); 

  rc = csd_dev_codec_deinit();
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_deinit: "
          "codec deinit returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,0); 

  rc = csd_dev_clk_deinit();
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_deinit: "
          "clk deinit returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CLK_INIT,0); 

  rc = csd_dev_afe_deinit();
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_deinit: "
          "afe deinit returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_AFE_INIT,0); 

  rc = csd_dev_tlmm_deinit();
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_deinit: "
          "tlmm deinit returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,0); 


  rc = csd_dev_oem_dinit();
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_tgt_deinit: "
          "csd_dev_oem_deinit returned with rc = 0x%x", rc);
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_OEM_INIT,0); 

  return rc;
}

int32_t csd_dev_codec_reset( void )
{
  /* suns - TODO: replace once Adie codec is available
  * return csd_dev_tgt_codec_adie_reset();
  */
  return CSD_EOK;
}

