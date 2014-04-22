/**
 * sns_pwr_sleep_mgr_usleep.c
 *
 * Usleep control for no-island platforms
 *
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *
 **/

#include "sns_island.h"
#include "sns_types.h"

void 
sns_pwr_sleep_mgr_disable_lpm_plus(int32_t unpa_client)
{
  UNUSED_VAR(unpa_client);
  return;
}

void 
sns_pwr_sleep_mgr_enable_lpm_plus(int32_t unpa_client)
{
  UNUSED_VAR(unpa_client);
  return;
}

int32_t
sns_pwr_sleep_mgr_create_unpa_handle(void)
{
  return 0;
}
