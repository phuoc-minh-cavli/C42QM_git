#ifndef CSD_DEV_SPEAKER_PROTECTION_INTF_
#define CSD_DEV_SPEAKER_PROTECTION_INTF_

/*-------------------------------------------------------------------------------
Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
Edit History
$Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/inc/csd_dev_speaker_protection.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/13   TBK     Initial revision
===============================================================================*/
#include "csd_dev_main.h"

/*
 * This function is used to check whether a device passed in is enabled with FB Speaker
 * Protection property.
 *
 * @param dev_id: id which is used to search
 *
 * @return CSD_EOK if device is enabled
 *         CSD_EFAIL if device is not found.
 */
int32_t csd_dev_fb_spkr_prot_is_fb_device(uint32_t dev_id);


/*
 * This function is used to get Tx Device Record of RX FB Speaker Protection Pair
 *
 * @param rx_device_record: rx device record.
 *        fb_tx_device_record: tx device record is returned.
 *
 * @return CSD_EOK if successful
 */
int32_t csd_dev_fb_spkr_prot_get_tx_device_record(const csd_dev_record_t *rx_device_record, 
  csd_dev_record_t **fb_tx_device_record );

/*
 * This function is used to prepare the cfg params of RX Device with Calibration mode FB Parameters
 *
 * @param new_dev: new device entry that is being enabled.
 *        spkr_calib_cfg: Fill the necessary cfg parameters the spkr protection
 *
 * @return CSD_EOK if successful
 */
int32_t csd_dev_fb_spkr_prot_init_cfg_params(struct csd_dev_entry *new_dev, 
  csd_dev_spkr_calib_ctrl_t* spkr_calib_cfg);

/*
 * This function is used to inform that speaker is enabled.
 *
 * @param dev_id: dev id that is enabled.
 *
 * @return.
 */
void csd_dev_fb_spkr_prot_device_enabled(int32_t dev_id);

/*
 * This function is used to inform that speaker is disabled.
 *
 * @param dev_id: dev id that is disabled.
 *
 * @return.
 */
void csd_dev_fb_spkr_prot_device_disabled(int32_t dev_id);

/*
 * This function is used to initialize the fb spkr module.
 *
 * @param dev_id: dev id that is disabled.
 *
 * @return.
 */
int32_t csd_dev_fb_spkr_prot_init(void);

/*
 * This function is used to de-initialize the fb spkr module.
 *
 * @param dev_id: dev id that is disabled.
 *
 * @return.
 */
int32_t csd_dev_fb_spkr_prot_deinit(void);

#endif
