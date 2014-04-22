#ifndef _CSD_DEV_CLK_H_
#define _CSD_DEV_CLK_H_
/*==============================================================================
  @file csd_dev_pmic.h

  The objective of this file is to provide prototype for interaction with
  clock module.

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_clk.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $
  $Id: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_clk.h#1 $

  when       who        what, where, why
  --------   -------    ----------------------------------------------------------
  04/27/010  suns       Initial Version
==============================================================================*/
#include "csd_dev_common.h"

/**
 * This function must be called to enable the corresponding clk for a particular audio path
 *
 * @param[in] a_dev  csd_device recored structure
 */
int32_t csd_dev_clk_enable(const csd_dev_record_t* a_dev);

/**
 * This function must be called to disable the corresponding clk for a particular audio path
 *
 * @param[in] a_dev  csd_device recored structure
 */
int32_t csd_dev_clk_disable(const csd_dev_record_t* a_dev);

/**
 * This function must be called to initialize CSD device clk component
 */
int32_t csd_dev_clk_init( void );

/**
 * This function must be called to deinit CSD device clk component.
 */
int32_t csd_dev_clk_deinit( void );

/**
* This function is used to Enable MCLK source via target specific implementation
* @param[in] enable : Boolean toggle switch to enable/disable
*/
int32_t csd_dev_clk_codec_mclk_control(bool_t enable);

#endif /* _CSD_DEV_CLK_H__ */
