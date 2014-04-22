#ifndef __CSD_DEV_COMMON_H__
#define __CSD_DEV_COMMON_H__
/*==============================================================================
  @file csd_dev_common.h

  This header contains structure and function skeletons which are commonly used
  for managing a device, especially the setup and tear down operation.

  Copyright (c) 2008-13 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/inc/csd_dev_common.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  10/07/13   vk      added support for AVSync feature
  02/15/13   suns    Device module unification.
  09/21/12   unni    B Family base
                     -Removed CSD_DEV_AUXPGS_DEVICE in csd_dev_direction_type
                      as it is no longer used.
                     -Reformatting to a common format
  04/27/10   suns    Initial Version
==============================================================================*/
#include "mmdefs.h"
#include "csd_dev_ioctl.h"
#include "csd_dev_oem_plugin_intf.h"
#include "csd_dev_device_info.h"

/* device ID   */
#define CSD_DEV_ID_INVALID            0xFFFFFFFF
#define CSD_DEV_INVALID_AFE_PORT_ID   0xFFFF

/*
 * Enumeration for supported device directions
 * Used as index for g_csd_dev_manager
 */
enum csd_dev_direction_type 
{
  CSD_DEV_RX_DEVICE,
  CSD_DEV_TX_DEVICE,
  CSD_DEV_TYPE_MAX
};

/*
 * Device operation status mask bits
 * First sixteen [1-16] bits with Mask [0x0000FFFF]
 */
#define CSD_DEV_STATUS_REG_MASK         0x00000001
#define CSD_DEV_STATUS_ANC_MASK         0x00000002
#define CSD_DEV_STATUS_SUSPEND_MASK     0x00000004

/* Cached device configuration : mainly used for suspend and resume */
#define CSD_DEV_AFE_LOOPBACK_SET        0x00000001
#define CSD_DEV_ANC_SET                 0x00000002
#define CSD_DEV_COMAPANDING_SET         0x00000004
#define CSD_DEV_DTMF_SET                0x00000008
#define CSD_DEV_SIDETONE_SET            0x00000010
#define CSD_DEV_HDMI_MULTICH_SET        0x00000020
#define CSD_DEV_AANC_SET                0x00000040
#define CSD_DEV_SPKR_PROT_SET           0x00000080

typedef enum e_spkr_calib_mode{
  CSD_NORMAL = 0x0,
  CSD_CALIBRATION = 0x1,
}e_spkr_calib_mode_t;

#define SPKR_TX_DEVICE_CHANNELS 1
typedef struct csd_dev_spkr_calib_ctrl{
  uint32_t dev_id;
  int32_t r0_q24;
  int16_t t0_q6;
  e_spkr_calib_mode_t spkr_mode;
  uint32_t fb_tx_dev_id[SPKR_TX_DEVICE_CHANNELS];
}csd_dev_spkr_calib_ctrl_t;

/* Device Configuration specific params */
typedef struct
{
  uint32_t                          control_params;
  /* Corresponding Cache Structures */
  csd_dev_afe_loopback_t            afe_loopback;
  csd_cmd_dev_anc_t                 anc_ctl;
  csd_dev_companding_t              cmp_ctl;
  csd_dev_dtmf_ctrl_t               dtmf_ctl;
  csd_dev_sidetone_ctrl_t           sidetone_ctl;
  csd_dev_hdmi_multi_channel_cfg_t  hdmi_multich_cfg;
  csd_cmd_dev_aanc_t                aanc_ctl;
  csd_dev_spkr_calib_ctrl_t         spkr_calib_cfg;
}csd_dev_cache_config_t;

/* CSD Device Record */
typedef struct csd_dev_record
{
  uint32_t               dev_id;           /* unique device ID */
  csd_dev_attrib_t       curr_attrib;      /* device's current attribute */
  int8_t                 manager_seq_index;/* current manager seq index */
  uint32_t               mask;             /* status mask bits */
/*==============================================================================
    Bit|     Mask |        Description
      1|0x00000001|regular operation status (0-Disabled, 1-Enabled)
      2|0x00000002|ANC operation status
      3|0x00000004|Device Active But currently suspended
   4~16|0x0000FFF8|reserved
     17|0x00010000|Device has Mapped remote (MDM) device ID
       |          |that needs to be setup on remote (MDM) path
     18|0x00020000|Device needs to setup Internal Router device
       |          |explicitly
     19|0x00040000|Device needs EC Reference to be setup
       |          |< Implicit for all voice RX devices (Dir= Rx and bit 9) >
     20|0x00080000|Device additionally needs to setup loopback on one of the
       |          |internal router device.
     21|0x00100000|Device is internal Router Device Type <Slimbus Device: with
       |          |no codec configuration: no Remoting >
     22|0x00200000|Device is EcoCancel(EC-Ref) Device Type <Slimbus Device:
       |          |with either (codec or AFE configuration) with Remoting >
  23~32|0xFFC00000|reserved
  ==============================================================================
 * All Non slimbus devices need AFE configuration on Both APQ and MDM
 * some slimbus devices like ICVR, ICMD need no codec configuration:
 * dedice based on codecPathId=0xFFFFFFFF
 */
  csd_dev_cfg_param_t     cfg_param;       /* TODO:Unni 20bytes, revise*/
  csd_acdb_dev_info_t     dev_info;        /* device info */
  csd_dev_cache_config_t  cache_cfg_param; /* TODO:Unni 66Bytes, revise*/
  uint32_t                av_sync_hw_delay;/* device HW delay */
  struct csd_dev_record*  next;            /* point to the next active record */
}csd_dev_record_t;

/* 
 * TODO:  @Unni-QNX Specific structure.. redundant with csd_acdb_dev_info 
 *        Need to merge to one common structure. This is now used in both
 *        audio and voice.
 */
typedef struct{
  uint32_t dev_interface_type;/* Type of Device
                                    (I2S/PCM/Slimbus/HDMI/Virtual) */
  uint32_t codec_path_id;     /* Associated codec_path_id */
  uint16_t afe_port_id;       /* Associated AfePortId (for I2S/PCM/etc) */
  uint32_t direction_mask;    /* Direction Mask for Device */
  uint32_t channel_config;    /* Channel configuration associated with device */
  uint32_t voice_topology_id; /* To be Deprecated (No More Used) */
  uint32_t audio_topology_id; /* To be Deprecated (No More Used) */
} csd_dev_info_t;

/* 
 * dev_control is a function pointer, used to point to a particular setup
 * or teardown function
 *
 * @param dev - reference to a device which will be used in the function
 *              refered by dev_control
 *
 * @return csd return status @see csd_status.h
 */
typedef int32_t (*dev_control)( const struct csd_dev_record *dev );

/*
 * This function is defined specific to how a device needs to be setup up
 *
 * @param new_dev     reference to a new device which needs to be setup
 * @param status_mask device status mask @see csd_dev_record
 *
 * @return csd return status @see csd_status.h
 */
int32_t csd_dev_setup( struct csd_dev_entry *new_dev, uint32_t status_mask );

/*
 * This function is defined specific to how a device needs to be tear down
 *
 * @param dev_id      id of the existing device
 * @param status_mask device status mask @see csd_dev_record
 *
 * @return csd return status @see csd_status.h
 */
int32_t csd_dev_teardown( uint32_t dev_id, uint32_t mask_bit );

/*
 * This function is used to search for an active csd_dev_record using device id
 *
 * @param dev_id      id which is used to search
 *
 * @return reference to csd_dev_record
 */
csd_dev_record_t* csd_dev_get_active_record( const uint32_t dev_id );

/*
 * This function is used to fetch device info and cache it
 *
 * @param dev_id      id which is used to search
 * @param dev_info    return device info
 *
 * @return reference to csd_dev_record
 */
int32_t csd_dev_get_dev_info(uint32_t dev_id, csd_dev_info_t* dev_info);

/*
 * This function is used to remove device info after use
 *
 * @param dev_id      id which is used to search
 *
 * @return reference to csd_dev_record
 */
int32_t csd_dev_remove_dev_info(uint32_t dev_id);

/*
 * This function is used to check if a sample rate is valid with the device
 *
 * @param dev_id      id which is used to search
 * @param sample_rate sample rate
 *
 * @return reference to csd_dev_record
 */
int32_t csd_dev_is_valid_sample_rate(uint32_t dev_id, uint32_t sample_rate);

/*
 * Fetch afe port id for a device from active device record list
 *
 * @param dev_id      id which is used to search
 * @param afe_port_id return afe port id
 *
 * @return reference to csd_dev_record
 */
int32_t csd_dev_get_active_afe_port_id(uint32_t dev_id,uint16_t* afe_port_id);

/*TODO: QNX specific functions, proper comments are missing*/
int32_t csd_dev_suspend(uint32_t dev_id);
int32_t csd_dev_restore(uint32_t dev_id);

int32_t csd_dev_apply_pre_dev_config(csd_dev_record_t* a_dev);
int32_t csd_dev_apply_post_dev_config(csd_dev_record_t* a_dev);
int32_t csd_dev_cache_dev_config(csd_dev_record_t* a_dev);

int32_t csd_dev_codec_reset( void );
int32_t csd_dev_get_audio_record_device_pair(uint32_t tx_dev_id, uint32_t *rx_dev_id);

int32_t csd_dev_cmd_enable_fb_spkr_prot_control(csd_dev_spkr_calib_ctrl_t* fb_spkr_calib_cfg);

/*TODO: End of QNX functions*/

#endif /* __CSD_DEV_COMMON_H__ */
