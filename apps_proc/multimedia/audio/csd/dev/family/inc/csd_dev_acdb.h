#ifndef _CSD_DEV_ACDB_H_
#define _CSD_DEV_ACDB_H_
/*==============================================================================
  @file csd_dev_acdb.h

  This header contains structure and function skeletons which are commonly used
  for interfacing with ACDB driver.

  Copyright (c) 2008-13 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_acdb.h#1 $
 $DateTime: 2018/12/25 22:39:07 $
 $Author: pwbldsvc $
 $Change: 17929171 $
 $Revision: #1 $
 $Id: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_acdb.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 10/07/13   vk        added support for AVSync feature
 01/10/13   rp        Added SideTone IIR filter support
 04/27/010  suns      Initial Version
==============================================================================*/
#include "csd_dev_common.h"

typedef struct csd_acdb_dev_pair_list_type
{
  uint32_t   noofdevs;
  uint32_t   *devList;
} csd_acdb_dev_pair_list_type_t;

/** Device specific ACDB init function: to be called from csd_dev_tgt_init() */
int32_t csd_dev_acdb_init(void);

/** Device specific ACDB init function: to be called from csd_dev_tgt_deinit() */
int32_t csd_dev_acdb_deinit(void);

/**
 * This function must be called to retrieve device info from ACDB
 *
 * @param[in] dev_id  input param to query device info 
 * @param[out] *info  output param to be filled with device info
 */
int32_t csd_dev_acdb_get_dev_info(uint32_t dev_id, void *info);

/**
 * This function must be called to retrieve device capability from ACDB 
 *
 * @param[out] *capabilities output param to be filled with device capabilities
 */
int32_t csd_dev_acdb_get_dev_cap(csd_dev_caps_t* capabilities,
                                 uint32_t entries_buf_size);

/**
 * This function must be called to retrieve the paired ANC TX devices  from ACDB
 * based on RX device id.
 *
 * @param[out] *p_tx_ids output param to be filled with the corresponding ANC tx
 * device IDs
 */
int32_t csd_dev_acdb_get_anc_devs(uint32_t rx_id, uint32_t *p_tx_ids);

/**
 * This function must be called to retrieve the sidetone data from the acdb for
 * a pair of devices.
 *
 * @param[out] *gain contains the sidetone gain.
 */
int32_t csd_dev_acdb_get_afe_sidetone(uint32_t rx_id, 
                                      uint32_t tx_id,
                                      bool_t* enable, 
                                      uint16_t *gain);

/**
 * This function must be called to retrieve the sidetone IIR 
 *  filter enable information from the acdb for a pair of
 *  devices.
 *
 * @param[out] *iir_enable contains the sidetone iir enable 
 *       info.
 */
int32_t csd_dev_acdb_get_afe_sidetone_iir_enable(uint32_t rx_id, 
                                                 uint32_t tx_id,
                                                 bool_t *iir_enable);

/**
 * This function must be called to retrieve the sidetone IIR 
 * @param[out] *buf_len contains the size of data returned.
 * @param[out] *iir_data contains the sidetone iir filter data.
 */
int32_t csd_dev_acdb_get_afe_sidetone_iir_data(uint32_t rx_id, 
                                               uint32_t tx_id,
                                               void* iir_data, 
                                               uint32_t buf_len);

/**
 * This function must be called to retrieve the afe cfg parameters
 *
 *
 */
int32_t csd_acdb_get_afe_common_data(uint32_t dev_id,
                                     uint32_t sample_rate,
                                     uint32_t afe_module_id, 
                                     uint32_t afe_param_id, 
                                     uint8_t *pbuffer, 
                                     uint32_t buffer_length, 
                                     uint32_t *bytes_filled);


/**
 * Get Rx to Tx Mapped AANC Device Pair
 *
 *
 */
int32_t csd_acdb_get_aanc_device_pair(uint32_t rx_dev_id, uint32_t *tx_dev_id);

/**
 * Get Tx to Rx Mapped Audio Record Device Pair ( for Echo Cancellation)
 *
 *
 */
int32_t csd_acdb_get_audio_record_device_pair(uint32_t tx_dev_id, uint32_t *rx_dev_id);

/**
 * This function must be called to retrieve the afe common table data from ACDB.
 */
int32_t csd_acdb_get_afe_common_table_data(uint32_t dev_id,
                                           uint32_t sample_rate,
                                           uint8_t *afe_data,
                                           uint32_t buf_size,
                                           uint32_t *buf_size_used);

/**
 * This function must be called to check if the supplied dev has the 
 * required feature enabled.
 */
int32_t csd_acdb_is_dev_feature_enabled(int32_t dev_id, uint32_t feature_id);

/**
 * This function must be called to retrieve the list of dev ids 
 * that are enabled with a specific feature.
 */
int32_t csd_acdb_get_devices_of_feature(uint32_t feature_id,
                                        uint8_t* p_cap_dev_list,
                                        uint32_t buf_size,
                                        uint32_t* buf_size_used);

/**
 * This function must be called to retrieve Tx device Mapped to FB Speaker Rx Device.
 */
int32_t csd_acdb_get_fb_spkr_prot_device_pair(uint32_t rx_dev_id, uint32_t *tx_dev_id);

/**
 * This function must be called to get device property size info from acdb for given device id and property.
 *
 * @param[in] dev_id  input param to query device property size info
 * @param[in] dev_prop_id input param to specify the device property
 * @param[out] *dev_size contains device property size info
 */
int32_t csd_acdb_get_dev_prop_size(uint32_t device_id,uint32_t dev_prop_id,
                                   uint32_t* dev_size);

/**
 * This function must be called to retrieve device property info from acdb for given device id and property.
 *
 * @param[in] dev_id  input param to query device property info
 * @param[in] dev_prop_id input param to specify the device property 
 * @param[out] *dev_prop_data contains device property info
 */
int32_t csd_acdb_get_dev_prop_info(uint32_t device_id,uint32_t dev_prop_id,
                                   uint32_t buffer_length,uint32_t *dev_prop_data);

/**
 * This function must be called to get device av sync hw delay from acdb for given device id 
 * and sample rate
 *
 * @param[in] dev_id  input param to get device hw delay
 * @param[in] sample_rate  input param to get device hw delay
 * @param[out] *hw_delay contains the device hw delay value for specified sample rate.
 */
int32_t csd_acdb_get_device_avsync_hw_delay(uint32_t device_id,uint32_t sample_rate,
                                            uint32_t* hw_delay);


#endif /* _CSD_DEV_ACDB_H_ */


