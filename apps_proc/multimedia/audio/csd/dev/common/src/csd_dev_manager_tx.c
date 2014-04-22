/*==============================================================================
  @file csd_dev_manager_tx.c

  This file defines some basic data structure used in csd tx device management

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
#include "csd_dev_manager_tx.h"

static csd_dev_manager* csd_dev_sm_tx[CSD_DEV_MANAGER_TYPE_MAX] =
{
  csd_dev_manager_tx_setup,
  csd_dev_manager_tx_teardown,
};

int32_t csd_dev_manager_tx_init(csd_dev_manager*** manager_db)
{
	manager_db[CSD_DEV_TX_DEVICE] = csd_dev_sm_tx;
	return CSD_EOK;
}

int32_t csd_dev_manager_tx_deinit(void)
{
  return CSD_EOK;
}

