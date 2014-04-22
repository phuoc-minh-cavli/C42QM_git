/*==============================================================================
  FILE:         uSleep_lpr_modes.c

  OVERVIEW:     This file contains the low power modes supported in island mode


  DEPENDENCIES: Object file generated from source is marked as island section

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/lpr/uSleep_lpr_modes.c#3 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_lpr.h"
#include "uSleep_lpri.h"
#include "DDITimetick.h"

/*==============================================================================
                             FORWARD DECLRATIONS
 =============================================================================*/
void uSleepLPR_cachedApcrEnter(uint64 wakeupTime, boolean reEntrant);
void uSleepLPR_cachedApcrPlusEnter(uint64 wakeupTime, boolean reEntrant);

void uSleepLPR_apcrEnter(uint64 wakeupTime, boolean reEntrant);
void uSleepLPR_apcrPlusEnter(uint64 wakeupTime, boolean reEntrant);
void uSleepLPR_apcrExit(void);

void uSleepLPR_fullPCEnter(uint64 wakeupTime, boolean reEntrant);
void uSleepLPR_fullPCExit(void);

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Supported LPR's in uSleep. */
static uSleep_LPR g_uSleepLowPowerModeTable[USLEEP_NUMBER_LOW_POWER_MODES] =
{
  /* Full Power collapse (LPM+)*/
  {"Full PC",
    {800, 2}, /* AON does it's own backoff */
    40,
    NULL, uSleepLPR_fullPCEnter, uSleepLPR_fullPCExit,
    FALSE,
    USLEEP_LPR_DISABLED}
};

/*==============================================================================
                              PUBLIC FUNCTIONS
 =============================================================================*/
/*
 * uSleepLPR_returnModes
 */
uSleep_LPR *uSleepLPR_returnModes(void)
{
  DalDeviceHandle *dalHandle;
  uint32          nMode;
  uint64          temp64;

  CORE_DAL_VERIFY(DalTimetick_Attach("SystemTimer", &dalHandle));

  /* This function is only run once, so convert the seed latency values here */
  for(nMode = 0; nMode < USLEEP_NUMBER_LOW_POWER_MODES; nMode++)
  {
    DalTimetick_CvtToTimetick64(dalHandle, g_uSleepLowPowerModeTable[nMode].seed_latency.enter, T_USEC, &temp64);
    g_uSleepLowPowerModeTable[nMode].seed_latency.enter = (uint32)temp64;

    DalTimetick_CvtToTimetick64(dalHandle, g_uSleepLowPowerModeTable[nMode].seed_latency.exit, T_USEC, &temp64);
    g_uSleepLowPowerModeTable[nMode].seed_latency.exit = (uint32)temp64;

    DalTimetick_CvtToTimetick64(dalHandle, g_uSleepLowPowerModeTable[nMode].backoff, T_USEC, &temp64);
    g_uSleepLowPowerModeTable[nMode].backoff = (uint32)temp64;
  }

  return g_uSleepLowPowerModeTable;
}

