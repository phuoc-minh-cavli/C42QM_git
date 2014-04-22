/*==============================================================================
  @file csd_dev_manager_rx.c

  This file defines some basic data structure used in csd rx device management

  Copyright (c) 2008-13 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
  
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  02/15/13   suns    Device module unification
  05/11/10   suns    Initial Creation
==============================================================================*/

#include "csd_dev_manager_rx.h"

static csd_dev_manager* csd_dev_sm_rx[CSD_DEV_MANAGER_TYPE_MAX] =
{
  csd_dev_manager_rx_setup,
  csd_dev_manager_rx_teardown,
};

int32_t csd_dev_manager_rx_init(csd_dev_manager*** manager_db)
{
  manager_db[CSD_DEV_RX_DEVICE] = csd_dev_sm_rx;
  return CSD_EOK;
}

int32_t csd_dev_manager_rx_deinit(void)
{
  return CSD_EOK;
}

