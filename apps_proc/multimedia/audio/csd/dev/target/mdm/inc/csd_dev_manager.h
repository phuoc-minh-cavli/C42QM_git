#ifndef _CSD_DEV_MANAGER_H_
#define _CSD_DEV_MANAGER_H_
/*==============================================================================
  @file csd_dev_manager.h

  This file defines some basic data structure used in csd device management

  Copyright (c) 2008-13, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11	   suns		Initial draft
============================================================================*/
#include "csd_dev_common.h"
#include "csd_os_dependencies.h"

enum csd_dev_stage {
    CSD_DEV_INIT=0,     /**< Stage to do device specific initialization */
    CSD_DEV_CLK,        /**< stage to do clock setup */
    CSD_DEV_TLMM,       /**< stage to do TLMM setup */
    CSD_DEV_CODEC_RAILS,/**< stage to do codec power rails setup */
    CSD_DEV_PATH,       /**< stage to do codec PATH setup */
    CSD_DEV_DSP_AFE,    /**< stage to do DSP setup */
    CSD_DEV_CODEC,      /**< stage to do codec analog enable(adie, i2s, auxpcm) */
    CSD_DEV_STAGE_MAX
};

typedef struct {
    enum csd_dev_stage  stage;
    dev_control         action;
}csd_dev_manager;

/**********************
CSD Device State Machine Type
***********************/
enum csd_dev_manager_type {
	CSD_DEV_MANAGER_SETUP = 0,
	CSD_DEV_MANAGER_TEARDOWN,
	CSD_DEV_MANAGER_TYPE_MAX
};

int32_t csd_dev_manager_up(struct csd_dev_record* a_dev, enum csd_dev_manager_type index);

int32_t csd_dev_manager_down(struct csd_dev_record* a_dev, enum csd_dev_manager_type index);

int32_t csd_dev_manager_init( void );

int32_t csd_dev_manager_deinit( void );

#endif /* _CSD_DEV_MANAGER_H_ */
