/*==============================================================================
  FILE:         apcr_lpr.c

  OVERVIEW:     This file provides uSleep APCR LPR functions

  DEPENDENCIES: Object file generated from source is marked as island section

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/lpr/apcr_lpr.c#1 $
$DateTime: 2019/02/20 03:28:54 $
==============================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "uSleep.h"
#include "q6_lpm_config.h"
#include "uSleep_target.h"
#include "uCoreTime.h"
#include "CoreVerify.h"

/*==============================================================================
                       INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
void uSleepLPR_programFullWakeup(uint64 wakeupTime)
{
  uint64                    AONWakeup;
  const uSleep_lpr_latency  *modeLat = uSleepTarget_getCurrentModeLatency();

  AONWakeup = wakeupTime - modeLat->exit;

  /* Program the wakeup times */
  uSleepTarget_programWakeupTimer(wakeupTime, AONWakeup);

  /* Final check to make sure times are valid */
  CORE_VERIFY(uCoreTimetick_Get64() < AONWakeup);

  return;
}

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
/*
 * uSleepLPR_apcrEnter
 */
void uSleepLPR_apcrEnter(uint64 wakeupTime, boolean reEntrant)
{
  /* Disable AON handshake */
  uSleepTarget_setRpmHandshake(FALSE);

  /* APCR mode with PMI interrupt since this is uncached */
  q6LPMConfig_setupModeConfig(RSC_MODE_APCR_PLL_ON, TRUE);
  
  /* Program only the local wakeup time */
  uSleepTarget_programWakeupTimer(wakeupTime, 0);

  /* Enter all wait */
  qurt_power_apcr_enter();

  return;
}

/*
 * uSleepLPR_apcrPlusEnter
 */
void uSleepLPR_apcrPlusEnter(uint64 wakeupTime, boolean reEntrant)
{
  /* Enable AON handshake */
  uSleepTarget_setRpmHandshake(TRUE);

  /* Configure for APCR */
  q6LPMConfig_setupModeConfig(RSC_MODE_APCR_PLL_LPM, TRUE);
	
  /* Set AON and local wakeups */
  uSleepLPR_programFullWakeup(wakeupTime);

    /* Enter all wait */
  qurt_power_apcr_enter();

  return;
}

/*
 * uSleepLPR_apcrExit
 */
void uSleepLPR_apcrExit(void)
{
  /* Disable wakeup timer */
  uSleepTarget_disableWakeupTimer();
  return;
}
#else

/*
 * uSleepLPR_cachedApcrEnter
 */
void uSleepLPR_cachedApcrEnter(uint64 wakeupTime, boolean reEntrant)
{
  if(TRUE != reEntrant)
  {
    /* Disable AON handshake */
    uSleepTarget_setRpmHandshake(FALSE);

    /* In APCR cached mode, we do not need to run an exit function until
     * we change to an un-cached mode so disable the PMI interrupt. */
    q6LPMConfig_setupModeConfig(RSC_MODE_APCR_PLL_LPM, FALSE);
  }

  /* Program only the local wakeup time */
  uSleepTarget_programWakeupTimer(wakeupTime, 0);

  return;
}

/*
 * uSleepLPR_cachedApcrPlusEnter
 */
void uSleepLPR_cachedApcrPlusEnter(uint64 wakeupTime, boolean reEntrant)
{
  if(TRUE != reEntrant)
  {
    /* Enable AON handshake */
    uSleepTarget_setRpmHandshake(TRUE);

    /* In APCR cached mode, we do not need to run an exit function until
     * we change to an un-cached mode so disable the PMI interrupt. */
    q6LPMConfig_setupModeConfig(RSC_MODE_APCR_PLL_LPM, FALSE);
  }

  /* Set AON and local wakeups */
  uSleepLPR_programFullWakeup(wakeupTime);

  return;
}

#endif /* USLEEP_SUPPORT_UNCACHED_APCR */

