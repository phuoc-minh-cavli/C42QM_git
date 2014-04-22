#ifndef _CSD_DEV_AFE_H_
#define _CSD_DEV_AFE_H_
/*==============================================================================
  @file csd_dev_afe.h

  The objective of this file is to provide prototype for interaction with Audio
  Front End module.

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_afe.h#3 $
$DateTime: 2019/03/14 20:28:08 $
$Author: pwbldsvc $
$Change: 18597212 $
$Revision: #3 $
$Id: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_afe.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/14   jk      Added support for CSD_DEV_CMD_RESTART   
03/04/13   suns    Device module unification.
01/10/13   rp      Added SideTone IIR filter support
04/27/010  suns    Initial Version
==============================================================================*/
#include "csd_dev_common.h"
#include "adsp_afe_service_commands.h"

#define CSD_DEV_AFE_SRC_PORT_HIGH   256   /* src port high byte */
#define CSD_DEV_AFE_SRC_PORT_LOW      1   /* src port low byte */

#define CSD_DEV_CODEC_MCLK_FREQ         9600000
/*! \fn int32_t csd_dev_afe_deinit()
    \brief This function starts a AFE HW port or create a pseudo port 
  if requested device is a virtual device.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_deinit(void);
/*! \fn int32_t csd_dev_afe_init()
    \brief This function starts a AFE HW port or create a pseudo port 
  if requested device is a virtual device.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_init(void);
/*! \fn int32_t csd_dev_q6_afe_start(const csd_dev_record_t* dev)
    \brief This function starts a AFE HW port or create a pseudo port 
  if requested device is a virtual device.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_start(const csd_dev_record_t* dev);

/*! \fn int32_t csd_dev_q6_afe_stop(const csd_dev_record_t* dev)
    \brief This function stop a AFE HW port or destroy a pseudo port 
  if requested device is a virtual device.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_stop(const csd_dev_record_t* dev);

/*! \fn int32_t csd_afe_enable_loopback(uint32_t rx_afe_id, 
                          uint32_t tx_afe_id, bool_t enable_flag,uint16_t mode)
    \brief This function enable/disable loopback between AFE interfaces.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_enable_loopback(uint16_t rx_afe_id, uint16_t tx_afe_id, 
                                    bool_t enable_flag, uint16_t mode);


/*! \fn int32_t csd_dev_afe_enable_sidetone(uint32_t rx_afe_id, uint32_t tx_afe_id, 
                        bool_t enable_flag,uint16_t gains)
    \brief This function enable/disable sidetone between AFE interfaces.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_enable_sidetone(uint16_t rx_afe_id, uint16_t tx_afe_id, 
                                    uint32_t enable_flag, uint16_t gains);

/*! \fn int32_t csd_dev_afe_set_dtmf_state(struct csd_dev_dtmf_ctrl * dtmf_ctrl)
    \brief This function enable/disable dtmf on AFE ports. 
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_set_dtmf_state(csd_dev_dtmf_ctrl_t* dtmf_ctrl);

/*! \fn int32_t csd_dev_afe_set_clock(uint16_t afe_port_id, 
                                                   csd_dev_clk_cfg_t *clk_cfg)
    \brief This function configures the LPASS interface clocks. 
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_set_clock(uint16_t afe_port_id,
                              const afe_param_id_clock_set_t *clk_cfg);

/*! \fn int32_t csd_dev_afe_enable_sidetone_iir(uint16_t tx_afe_id, bool_t enable_flag)
    \brief This function enable/disable sidetone IIR filter between AFE interfaces.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_enable_sidetone_iir(uint16_t tx_afe_id, bool_t enable_flag);

/*! \fn int32_t csd_dev_afe_send_sidetone_iir_data(uint16_t tx_afe_id,q6msg_config_iir_filter* iir_data)
    \brief This function configures sidetone IIR filter data through AFE interfaces.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_send_sidetone_iir_data(uint32_t rx_dev_id,
                                           uint32_t tx_dev_id,
                                           uint16_t tx_afe_id );

/*! \fn int32_t csd_dev_afe_hdmi_multi_channel_config(
                                const csd_dev_record_t* dev,
                                csd_dev_hdmi_multi_channel_cfg_t* hdmi_cfg)
    \brief This function enable/disable dtmf on AFE ports.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_hdmi_multi_channel_config(const csd_dev_record_t* dev,
                              const csd_dev_hdmi_multi_channel_cfg_t* hdmi_cfg);


/*! \fn int32_t csd_dev_afe_get_aanc_enable_mode(uint32_t tx_dev_id, bool_t *pEnable)
    \brief This is helper function to get AANC enable mode as specified in the ACDB
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_get_aanc_enable_mode(uint32_t tx_dev_id, bool_t *pEnable);

/*! \fn int32_t csd_dev_afe_aanc_configure(const struct csd_dev_record* ref_dev, 
                        const struct csd_dev_record* tx_dev)
    \brief This function Configures the AANC Params on a Afe port of TX device and info from Ref Device Afe port.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_aanc_configure(const csd_dev_record_t* ref_dev,
                                   const csd_dev_record_t* tx_dev);

/*! \fn csd_dev_afe_aanc_control(uint32_t tx_afe_id, bool_t aanc_enable_mode)
    \brief This function sets (enables or disables) AANC setup on a (TX) Port  
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_aanc_control(uint32_t tx_afe_id, bool_t aanc_enable_mode);

/*! \fn int32_t csd_dev_afe_get_fb_spkr_prot_calibrated_r0(const struct csd_dev_record* tx_dev,
                        int32_t *r0)
    \brief This function gets the calibrated R0 for a given  FB Speaker Protection enabled TX device.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_get_fb_spkr_prot_calibrated_r0(const csd_dev_record_t *tx_dev, int32_t *r0);
/*! \fn int32_t csd_dev_afe_restart(
                                uint16_t rx_port_id,
                                uint16_t tx_port_id,
                                uint32_t sample_rate)
    \brief This function Restarts AFE ports.
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_restart(uint16_t rx_port_id,
                            uint16_t tx_port_id,
                            uint32_t sample_rate );
/*! \fn int32_t csd_dev_afe_set_lpass_shared_clock(uint16_t afe_port_id, 
                    const afe_param_id_lpass_core_shared_clk_cfg_t *clk_cfg)
    \brief This function enables LPASS shared core clock 
    \return int32_t CSD return codes
*/
int32_t csd_dev_afe_set_lpass_shared_clock(uint16_t afe_port_id, const afe_param_id_lpass_core_shared_clk_cfg_t *clk_cfg);
                            
                            
#endif /* _CSD_DEV_AFE_H_ */

