#ifndef _CSD_DEV_TLMM_H_
#define _CSD_DEV_TLMM_H_
/*==============================================================================
  @file csd_dev_tlmm.h

  The objective of this file is to provide prototype for interaction with
  tlmm module.

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_tlmm.h#3 $
  $DateTime: 2019/03/14 20:28:08 $
  $Author: pwbldsvc $
  $Change: 18597212 $
  $Revision: #3 $
  $Id: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_tlmm.h#3 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  27/04/10  suns		Initial version
============================================================================*/
#include "csd_dev_common.h"

/**
 * This function must be called to enable the corresponding GPIOs for a particular device.
 *
 * @param[in] a_dev  csd_device recored structure
 */
int32_t csd_dev_tlmm_enable(const csd_dev_record_t* a_dev);

/**
 * This function must be called to disable the corresponding GPIOs for a particular device.
 *
 * @param[in] a_dev  csd_device recored structure
 */
int32_t csd_dev_tlmm_disable(const csd_dev_record_t* a_dev);

/**
 * This function must be called to init TLMM
 */
int32_t csd_dev_tlmm_init(void);

/**
 * This function must be called to dinit TLMM
 */
int32_t csd_dev_tlmm_deinit(void);

/**
 * This function must be called to config codec reset gpio as output.
 */
int32_t csd_dev_reset_codec_gpio(void);

/**
 * This function must be called to reset codec reset gpio configuration.
 */
int32_t csd_dev_release_reset_codec_gpio(void);

/**
* This function is used to Enable MCLK GPIO via target specific implementation
* @param[in] enable : Boolean toggle switch to enable/disable
*/
int32_t csd_dev_tlmm_codec_mclk_gpio_control(bool_t enable);

int32_t csd_dev_tlmm_cdc_buck_gpio_control(bool_t enable);

int32_t csd_dev_tlmm_cdc_ldo_gpio_control(bool_t enable);

int32_t csd_dev_tlmm_cdc_pmic_gpio_control(bool_t enable);





#endif /* _CSD_DEV_TLMM_H_ */
