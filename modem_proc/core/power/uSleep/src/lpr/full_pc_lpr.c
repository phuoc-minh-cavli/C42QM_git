/*==============================================================================
  FILE:         full_pc_lpr.c

  OVERVIEW:     This file provides uSleep full powre collapse LPR functions

  DEPENDENCIES: None

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/lpr/full_pc_lpr.c#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_lpr.h"
#include "uSleep_log.h"
#include "uCoreTime.h"
#include "qurt.h"
#include "uSleep_target.h"
#include "uSleep_util.h"
#include "q6_lpm_config.h"
#include "rpm.h"
#include "rpmclient.h"
#include "vmpm_utils.h"

/*==============================================================================
                              EXTERNAL PROTOTYPES
 =============================================================================*/
void uSleepLPR_programFullWakeup(uint64 wakeupTime);

/*==============================================================================
                          INTERNAL GLOBAL VARIABLES
 =============================================================================*/
/* Debug/Test variable to count the number of times entered */
uint32 g_fullPCEntryCnt = 0;

volatile boolean g_isRPMAssisted = FALSE;
static uint32 g_nasResult;
static uint32 g_ssResult;

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepLPR_fullPCEnter
 */
void uSleepLPR_fullPCEnter(uint64 wakeupTime, boolean reEntrant)
{
  /* Configure for full PC */
  q6LPMConfig_setupModeConfig(RSC_MODE_PWRC, FALSE);

  /* Enable WL-AON handshake */
  uSleepTarget_setRpmHandshake(TRUE);

  g_isRPMAssisted = TRUE;
  
  /* Increase the entry count */
  g_fullPCEntryCnt++;

  /* Program the wakeup times */
  //uSleepLPR_programFullWakeup(wakeupTime);
  /* Tell RPM the wakeup time */
  vmpm_SetWakeupTimetick(wakeupTime);
   
  g_ssResult = rpm_force_sync(RPM_SLEEP_SET);

 /* sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_INFO, SLEEP_SET_SEND_NUM_ARGS,
                       SLEEP_SET_SEND_STR, SLEEP_SET_SEND,
                       ULOG64_DATA(wakeup_tick) ); */
  
  uSleepLog_QDSSPrintf(USLEEP_SET_SEND_NUM_ARGS,
                       USLEEP_SET_SEND_STR, 
                       USLEEP_SET_SEND,
                       ULOG64_DATA(wakeupTime));  

  g_nasResult = rpm_force_sync(RPM_NEXT_ACTIVE_SET);

  if(g_ssResult || g_nasResult)
  {
    rpm_mask_interrupt(true);
  }

  /* Enter all wait */
  qurt_power_shutdown_enter(QURT_POWER_SHUTDOWN_TYPE_L2NORET);

  uSleep_setLastSleepWakeupInternal(uCoreTimetick_Get64());

  return;
}

/*
 * uSleepLPR_fullPCExit
 */
void uSleepLPR_fullPCExit(void)
{
  /* Disable timer */
 // uSleepTarget_disableWakeupTimer();
 
 /* Ensure churn_queue uses g_nasResult if valid, else g_ssResult */
  rpm_churn_queue(g_nasResult?g_nasResult:g_ssResult);

  if(g_ssResult || g_nasResult)
  {
    rpm_mask_interrupt(false);
  }

  uSleepTarget_setRpmHandshake(FALSE);
   
  /* Configure the Q6 for Clock Gating when exiting any LPRM */
  q6LPMConfig_setupModeConfig(RSC_MODE_CLK_GATE, FALSE);

  return;
}

