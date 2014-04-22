#ifndef _CSD_DEV_OEM_H_
#define _CSD_DEV_OEM_H_
/*==============================================================================
  @file csd_dev_oem_plugin.h

  This header contains structure default device list enumration.

                Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
                All rights reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when        who      what, where, why
--------    ---      ----------------------------------------------------------
04/9/13     jk      Changed name to match QNX
02/14/13    unni    Updated device unified list; synced Audio_Device_IDs.xlsx#2
09/19/11    oa      removed unused devices on TN1.0, added tty devices
06/02/11    oa      initial draft for 9x15
============================================================================*/

//TODO: To be deleted once ftm_audio_dispatch.c dependencies are clear.
#define CSD_OEM_DEV_I2S_PRI_MASTER_TX_MONO 0x0
#define CSD_OEM_DEV_I2S_PRI_MASTER_RX_MONO 0x1

/* 
 * OEM published device ID List
 * src: //source/qcom/qct/multimedia2/Audio/platforms/MDM/BFamily/docs/acdb/
 * Audio_Device_IDs.xlsx
 */
enum csd_oem_device_id 
{
  HANDSET_MIC                       = 0x00000004,
  HANDSET_MIC_ENDFIRE               = 0x00000006,
  HANDSET_SPKR                      = 0x00000007,
  HEADSET_MIC                       = 0x00000008,
  HEADSET_SPKR_MONO                 = 0x00000009,
  HEADSET_SPKR_STEREO               = 0x0000000A,
  SPKRPHONE_MIC                     = 0x0000000B,
  SPKRPHONE_MIC_ENDFIRE             = 0x0000000D,
  SPKRPHONE_SPKR_MONO               = 0x0000000E,
  TTY_HEADSET_MIC                   = 0x00000010,
  TTY_HEADSET_SPKR                  = 0x00000011,
  BT_SCO_MIC                        = 0x00000014,
  BT_SCO_SPKR                       = 0x00000015,
  VCO_HANDSET_MIC                   = 0x00000024,
  HCO_HANDSET_SPKR                  = 0x00000025,
  BT_SCO_WB_MIC                     = 0x00000026,
  BT_SCO_WB_SPKR                    = 0x00000027,
  HANDSET_MIC_FLUENCEV5             = 0x00000032,
  HANDSET_MIC_ENDFIRE_FLUENCEV5     = 0x00000033,
  SPKRPHONE_MIC_FLUENCEV5           = 0x00000034,
  SPKRPHONE_MIC_ENDFIRE_FLUENCEV5   = 0x00000035,
  USB_TX                            = 0x00000036,
  USB_RX                            = 0x00000037,
  PRIMARY_PCM_TX                    = 0x00000038,
  PRIMARY_PCM_RX                    = 0x00000039,
  SECONDARY_PCM_TX                  = 0x0000003A,
  SECONDARY_PCM_RX                  = 0x0000003B,
  RT_PROXY_TX                       = 0x0000003C,
  RT_PROXY_RX                       = 0x0000003D,
};

/*
 * MAX DEVICE ID + 1
 */
#define CSD_OEM_DEVICE_ID_MAX_NUM   0x3E

#endif /* _CSD_DEV_OEM_H_ */
