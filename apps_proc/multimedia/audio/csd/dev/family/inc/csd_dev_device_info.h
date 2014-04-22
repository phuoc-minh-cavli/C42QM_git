#ifndef _CSD_DEV_DEVICE_INFO_H_
#define _CSD_DEV_DEVICE_INFO_H_
/*==============================================================================
  @file csd_dev_device_info.h
  
  This header contains the device structure definition. The majority of the 
  structures are depended on ACDB module. Since acdb returns data in a buffer
  referred by void*, you can uses these structures to make sense of the data.
  
  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_device_info.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  02/25/13   suns    Device Module unification.
  09/14/12   unni    B Family base code
                     -Reformatting to a common format
  07/13/12   rp      Added 9x25 AFE support.
==============================================================================*/

#include "csd.h"
#include "acdb.h"

/** Define Device Interface specific structures and Macros */
#define CSD_DEV_INVALID_CODEC_PATH_ID             (0)
#define CSD_DEV_AFE_PORT_MAX_AUDIO_CHAN_CNT       (8)

/*
 * As per definition Major payload versions redefine the structure to be used
 * for different device categories while the minor version indicates what
 * increment of the binary backwards compatible structure. Say assuming CSD
 * supports major version 1 minor version 2 -ACDB returns major version 1 minor
 * version 3, that structure is supported since the minor version is binary 
 * backwards compatible -ACDB returns major version 1 minor version 1, that 
 * structure is NOT supported since the minor version required by CSD is major
 * than the version provided by ACDB -ACDB returns major version 2 minor 
 * version 2, that structure is NOT supported since the major version change 
 * indicates the structure was created from scratch and most likely the members
 * of the structure are not the same.
 */

/* 
 * Structure definition of the data present in buffer got from ACDB. For 
 * supported values and variable usage directions, appropriate modules
 * are referred along side the varaibles
 */
/* I2S Device info structure */
typedef struct
{
  uint32_t codec_path_id;             /* @see codec docs */
  uint32_t afe_channel_mode;          /* @see AFE interface */
  uint32_t afe_mono_stereo;           /* @see AFE interface */
  uint32_t afe_word_sync_src_select;  /* @see AFE interface */
  uint32_t i2s_mclk_mode;             /* @see enum AcdbI2sMClkModeType */
  uint32_t sample_rate_mask;          /* @see enum AcdbSampleRateMaskType */
  uint32_t afe_data_type;             /* @see AFE interface */
} csd_acdb_dev_i2s_info_t;

/* PCM Device info structure */
typedef struct
{
  uint32_t afe_aux_mode;              /* PCM mode (0) AUX_PCM mode (1) */
  uint32_t afe_sync_src_select;       /* Src_external(0) src_internal (1) */
  uint32_t afe_frame_setting;         /* bits width per frame: 
                                         8 bits  (0),
                                         16 bits (1),
                                         32 bits (2),
                                         64bits  (3),
                                         128bits (4),
                                         256 bits(5) */
  uint32_t afe_quant_type;            /* Quant type: 
                                         A-law no padding  (0),
                                         Mu-law no padding (1),
                                         linear no padding (2),
                                         A-law padding     (3),
                                         Mu-law padding    (4),
                                         linear padding    (5) */
  uint32_t afe_data_out_enable_ctrl;  /* Output Data Enable Control: 
                                         tri-state - disable (0) 
                                         tri-state - enable (1) */
  uint16_t afe_slot_number_mapping[4];/* @see AFE interface */
  uint32_t mux_sel;                   /* @see lpass doc. Specifies the
                                         lpaif mux path i.e. one of the
                                         4 MI2S lines [1,2,3,4]  */
} csd_acdb_dev_pcm_info_t;

/* Slimbus Device info structure */
typedef struct
{
  uint32_t codec_path_id;             /* @see codec docs */
  uint32_t afe_slimbus_dev_id;        /* @see AFE interface */
  uint32_t afe_data_format;           /* @see AFE interface */
  uint8_t afe_shared_channel[CSD_DEV_AFE_PORT_MAX_AUDIO_CHAN_CNT];
                                      /* @see AFE interface */
} csd_acdb_dev_slimbus_info_t;

/* DMIC Device info structure */
typedef struct
{
  uint32_t afe_channel_mode;          /* @see AFE interface */  
  uint32_t master_clk_mode;           /* @see AcdbI2sMClkModeType enum type */
} csd_acdb_dev_dmic_info_t;

/* HDMI Device info structure */
typedef struct
{
  uint32_t afe_channel_mode;          /* @see AFE interface */
  uint32_t afe_data_type;             /* linear data type (0), 
                                         non-linear data type (1) */ 
} csd_acdb_dev_hdmi_info;

/* Virtual Device info structure */
typedef struct
{
  uint32_t afe_timing_mode;           /* TIMING_MODE_FTRT   0x0 -> 
                                         timing operate faster than real time
                                         TIMING_MODE_TIMER  0x1 -> 
                                         timing operate at system timer rate */
  uint32_t afe_data_type;             /* @see AFE interface */
} csd_acdb_dev_virtual_info_t;

/* BTFM Device info structure */ 
typedef struct
{
  uint32_t reserved;
} csd_acdb_dev_btfm_info_t;

/* RT Proxy Device info structure */
typedef struct
{
  uint32_t reserved;
} csd_acdb_dev_rtproxy_info_t;

/* Simple Device info structure */
typedef struct
{
  uint32_t sample_rate_mask;          /* @see the enum AcdbSampleRateMaskType */
  uint32_t channel_config;            /* @see the enum AcdbChannelConfigType */
  uint32_t bits_per_sample_mask;      /* @see the enum 
                                         AcdbAfeBytesPerSampleType */
} csd_acdb_dev_simple_info_t;

/* Union of all the devices info structures mentioned in this file*/
typedef union
{
  csd_acdb_dev_i2s_info_t i2s_dev_info;
  csd_acdb_dev_pcm_info_t pcm_dev_info;
  csd_acdb_dev_slimbus_info_t slimbus_dev_info;
  csd_acdb_dev_dmic_info_t dmic_dev_info;
  csd_acdb_dev_hdmi_info hdmi_dev_info;
  csd_acdb_dev_virtual_info_t virtual_dev_info;
  csd_acdb_dev_btfm_info_t btfm_dev_info;
  csd_acdb_dev_rtproxy_info_t rtproxy_dev_info;
  csd_acdb_dev_simple_info_t simple_dev_info;
} csd_acdb_dev_all_info_t;


/*
 * Structure for easy access of device information. All device specific
 * information specific to a device type is present in variable 'u'. Commonly
 * used parameters pertaining to a device is present in rest of the variables
 */
typedef struct
{
  uint8_t  is_virtual;            /* flag for virtual(0)/real(1) device */
  uint8_t  direction;             /* @see enum csd_dev_direction_type */
  uint16_t afe_port_id;
  uint32_t interface_type;        /* @see acdb.h ACDB_DEVICE_TYPE_..._PARAM */
  uint32_t sample_rate_mask;      
  uint32_t bits_per_sample_mask;  
  uint32_t channel_config;
  csd_acdb_dev_all_info_t u;
} csd_acdb_dev_info_t;



#endif /* _CSD_DEV_DEVICE_INFO_H_ */


