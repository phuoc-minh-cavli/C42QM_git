#ifndef _CSD_DEV_PMIC_H_
#define _CSD_DEV_PMIC_H_
/*==============================================================================
  @file csd_dev_pmic.h

  The objective of this file is to provide prototype for interaction with
  pmic module.

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_pmic.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $
  $Id: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_pmic.h#1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/04/13   unni    deprecating csd_dev_pmic_enable_device and introduction 
										 csd_dev_pmic_enable_basic_codec_rails
  04/19/10   suns    initial draft
==============================================================================*/
#include "csd_dev_common.h"

/**
 * Enable basic power rail for codec based device
 *
 * @param[in] a_dev  csd_device recorded structure
 */
int32_t csd_dev_pmic_enable_basic_codec_rails( void );

/**
 * Disable basic power rail for codec based device
 *
 * @param[in] a_dev  csd_device recorded structure
 */
int32_t csd_dev_pmic_disable_basic_codec_rails( void );

/**
 * Initailize pmic 
 */
int32_t csd_dev_pmic_init( void );

/**
 * De-initailize pmic 
 */
int32_t csd_dev_pmic_deinit( void );

/**
 * Enable the corresponding power rail for a particular device
 *
 * @param[in] a_dev  csd_device recorded structure
 */
int32_t csd_dev_pmic_enable_codec_rails(const csd_dev_record_t* a_dev);

/**
 * Disable the corresponding power rail for a particular device
 *
 * @param[in] a_dev  csd_device recorded structure
 */
int32_t csd_dev_pmic_disable_codec_rails(const csd_dev_record_t* a_dev);

/**
 * Enable the corresponding PMIC GPIO's rail for a particular device
 *
 * @param[in] a_dev  csd_device recorded structure
 */
/*TODO: Unni, is this function used to enable gpio. Shouldn't it be in TLMM module*/
int32_t csd_dev_pmic_enable_codec_gpios(const csd_dev_record_t* a_dev);

/**
 * Disable the corresponding PMIC GPIO's rail for a particular device
 *
 * @param[in] a_dev  csd_device recorded structure
 */
/*TODO: Unni, is this function used to enable gpio. Shouldn't it be in TLMM module*/
int32_t csd_dev_pmic_disable_codec_gpios(const csd_dev_record_t* a_dev);

#endif /* _CSD_DEV_PMIC_H_ */
