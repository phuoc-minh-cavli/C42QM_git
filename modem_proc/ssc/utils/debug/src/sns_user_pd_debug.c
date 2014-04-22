/*=============================================================================
  @file sns_user_pd_debug.c

  @brief
  Handle User PD debug options

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include <stdbool.h>
#include <stdint.h>

/*=============================================================================
  Data
  ===========================================================================*/
#if defined(SNS_DELAY_INIT)
bool sns_init_delayed = 1;
#else
bool sns_init_delayed = 0;
#endif

/* Max delay 15 seconds */
uint32_t sns_delay_sec = 7;

