/*==============================================================================
  FILE:         client_API.c

  OVERVIEW:     This file provides the public uSleep API functions 

  DEPENDENCIES: None
  
                Copyright (c) 2016-2019 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/client/client_API.c#2 $
$DateTime: 2019/03/15 09:12:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep.h"
#include "uSleep_lpr.h"
#include "uSleep_log.h"
#include "uATS.h"

/*==============================================================================
                       EXTERNAL GUEST SECTION FUNCTIONS
 =============================================================================*/
uSleep_status uSleep_setXOSettlingTime(uint32 ticks)
{
  static uint32 previousSetting = 0;

  uSleep_LPR *lpr = &g_uSleepLPR[USLEEP_MODE_FULL_PC];

  uSleepLog_printf(1,
                   " Setting XO crystal time "
                   "(ticks: 0x%x)",
                   ticks);

  /* Modify the working latency of the FULL PC (XO shutdown) LPR with the
   * given crystal delay */
  lpr->working_latency.exit -= previousSetting;
  lpr->working_latency.exit += ticks;

  /* save the last adjustment */
  previousSetting = ticks;

  /* Let the solver know that latency changed */
  uATS_SetSignal(USLEEP_SIGNAL_XO_SETTLE_CHANGE);

  return USLEEP_SUCCESS;
}

