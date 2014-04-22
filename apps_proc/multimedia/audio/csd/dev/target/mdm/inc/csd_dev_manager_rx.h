#ifndef _CSD_DEV_MANAGER_RX_H_
#define _CSD_DEV_MANAGER_RX_H_
/*==============================================================================
  @file csd_dev_manager_rx.h

  This file defines some basic data structure used in csd device management

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 05/11/10   suns   Initial Creation
 11/30/11   oa     Slimbus v2 implementation
==============================================================================*/
#include "csd_dev_manager.h"
#include "csd_dev_clk.h"
#include "csd_dev_tlmm.h"
#include "csd_dev_codec.h"
#include "csd_dev_acdb.h"
#include "csd_dev_afe.h"
#include "csd_dev_pmic.h"


/* manager for device setup */
static csd_dev_manager csd_dev_manager_rx_setup[CSD_DEV_STAGE_MAX] =
{
  { CSD_DEV_INIT,         csd_dev_codec_init_path}, /*Initialized the memory structures for codec and device configuration*/
  { CSD_DEV_TLMM,         csd_dev_tlmm_enable},
  { CSD_DEV_CLK,          csd_dev_clk_enable},
  { CSD_DEV_CODEC_RAILS,  csd_dev_pmic_enable_codec_rails},
  { CSD_DEV_PATH,         csd_dev_codec_procure_path},
  { CSD_DEV_DSP_AFE,      csd_dev_afe_start},
  { CSD_DEV_CODEC,        csd_dev_codec_enable},
};

/* manager for device tear down */
static csd_dev_manager csd_dev_manager_rx_teardown[CSD_DEV_STAGE_MAX] =
{
  { CSD_DEV_INIT,         csd_dev_codec_deinit_path},
  { CSD_DEV_TLMM,         csd_dev_tlmm_disable},
  { CSD_DEV_CLK,          csd_dev_clk_disable},
  { CSD_DEV_CODEC_RAILS,  csd_dev_pmic_disable_codec_rails},
  { CSD_DEV_PATH,         csd_dev_codec_release_path},
  { CSD_DEV_DSP_AFE,      csd_dev_afe_stop},
  { CSD_DEV_CODEC,        csd_dev_codec_disable},
};


int32_t csd_dev_manager_rx_init(csd_dev_manager*** manager_db);
int32_t csd_dev_manager_rx_deinit(void);

#endif /* _CSD_DEV_MANAGER_RX_H_ */
