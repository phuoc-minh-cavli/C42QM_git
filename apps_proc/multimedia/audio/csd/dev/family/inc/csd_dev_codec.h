#ifndef _CSD_DEV_CODEC_H_
#define _CSD_DEV_CODEC_H_
/*==============================================================================
  @file csd_dev_codec.h

  OVERVIEW:     The objective of this file is to provide prototype for interact-
                ion with codec module.

  Dependencies: @see

                Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
                All rights reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_codec.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $
  $Id: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_codec.h#1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  07/13/12   up      9x25 base.
  07/13/12   rp      Added 9x25 AFE support.
  04/27/010  suns    Initial Version
============================================================================*/

#include "csd_dev_common.h"
#include "wcd_result.h"

#define CSD_MAX_ADIE_CHANNELS 8

typedef struct device_sink_info
{
  uint32_t number_of_paths;
  int32_t sinks[CSD_MAX_ADIE_CHANNELS];
}device_sink_info_t;

/**
 * This function must be called to init csd Device codec component.
 */
int32_t csd_dev_codec_init_path(const csd_dev_record_t* dev);

/**
 * This function must be called to init csd Device codec component.
 */
int32_t csd_dev_codec_deinit_path(const csd_dev_record_t* dev);

/**
 * This function must be called to create an audio path for a particluar device
 *
 * @param[in] dev  csd_device recored structure
 */
int32_t csd_dev_codec_procure_path(const csd_dev_record_t* dev);

/**
 * This function must be called to create an audio path for a particluar device
 *
 * @param[in] dev  csd_device recored structure
 */
int32_t csd_dev_codec_release_path(const csd_dev_record_t* dev);

/**
 * This function must be called to enable an audio path for a 
 * particluar device
 *
 * @param[in] dev  csd_device recored structure
 */
int32_t csd_dev_codec_enable(const csd_dev_record_t* dev);

/**
 * This function must be called to disable an audio path for a 
 * particluar device
 *
 * @param[in] dev  csd_device recored structure
 */
int32_t csd_dev_codec_disable(const csd_dev_record_t* dev);

/**
 * This function must be called to init csd Device codec component.
 */
int32_t csd_dev_codec_init(void);

/**
 * This function must be called to deinit csd Device codec component.
 */
int32_t csd_dev_codec_deinit(void);

/**
 * This function is used to setup [Enable or Disable] ANC/AANC Feature on 
 * ADIE Codec. 
 */
int32_t csd_dev_codec_adie_anc_control(const csd_dev_record_t* rx_dev, 
                                       const csd_dev_record_t* tx_dev,
                                       bool_t aanc_enable_mode);

/**
* This function is used to get the usage ADIE handle based on the csd-dev record
*/
int32_t csd_dev_codec_get_adie_usage_handle(const csd_dev_record_t* dev, void **return_handle);

/**
* This function is used to setup the sidetone filter on the given 
* RX device using a given Tx Device
*/
int32_t csd_dev_codec_sidetone_ctrl(const csd_dev_record_t* rx_dev, 
                                    const csd_dev_record_t* tx_dev, 
                                    const csd_dev_sidetone_ctrl_t* sidetone);


/**
* This function is used to get the sink paths for a given codec path id.
* Pointer to codec param id is passed in.
*/
int32_t csd_dev_codec_get_sink_paths(uint32_t codec_path_id, 
                                     device_sink_info_t* sink_info);

/**
* This function is used to get the codec param id.
* Pointer to codec param id is passed in.
*/
int32_t csd_adie_get_acdb_codec_adie_profile_param_id(uint32_t *codec_param_id);

/**
* This is a cb function to reset the codec.
*/
wcd_result csd_dev_wcd_cb(void* payload, uint32 payload_size, void* client_data);

#endif /* _CSD_DEV_CODEC_H_ */
