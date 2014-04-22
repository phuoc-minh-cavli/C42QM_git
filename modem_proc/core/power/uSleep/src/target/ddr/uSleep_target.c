/*==============================================================================
  FILE:         uSleep_target.c

  OVERVIEW:     This file provides uSleep target specfic functions while in
                normal operational mode

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/target/ddr/uSleep_target.c#4 $
$DateTime: 2019/07/29 00:49:30 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep.h"
#include "uSleep_target.h"
#include "uSleep_os.h"
#include "rsc_common.h"
#include "DDIInterruptController.h"
#include "DDITimetick.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "vmpm.h"
#include "rsc.h"
#include "timer.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
//#include "sleep_pmi.h"
#endif

/*==============================================================================
                               GLOBAL VARIABLES
 =============================================================================*/
#ifdef USLEEP_SUPPORT_TRANSITIONS
/* If TRUE, allows island debugging by keeping the JTAG port active. */
static volatile boolean g_usleepEnableIslandDebug = FALSE;
#endif

/*==============================================================================
                            EXTERNAL VARIABLES
 =============================================================================*/
extern DalDeviceHandle *g_DALWakeTimerHandle;
/* deferrable timer groups */
extern timer_group_type rex_timer_group;
extern timer_group_type timer_null_group;

/*==============================================================================
                            INTERNAL DECLRATIONS
 =============================================================================*/
/* Dal data struct to hold DAL Interrupt Controller Handle */
static DalDeviceHandle *g_DALInterruptCtrlHndl;

/*==============================================================================
                            FORWARD DECLRATIONS
 =============================================================================*/
void sleepTarget_wakeupHandler(uint32 param);

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#ifdef USLEEP_SUPPORT_TRANSITIONS
/*
 * uSleepTarget_enableIslandDebug
 */
void uSleepTarget_enableIslandDebug(boolean enable)
{
  static boolean allowed = FALSE;

  if((TRUE == g_usleepEnableIslandDebug) && (TRUE == enable))
  {
    /* Enable island debug */
    allowed = TRUE;
  }
  else if(TRUE == allowed)
  {
    /* Disable island debug */
    allowed = FALSE;
  }

  return;
}

/*
 * uSleepTarget_setPMIHandler
 */
void uSleepTarget_setPMIHandler(void)
{
  /* Use main sleep's API to set the PMI function pointer */
  sleepPMI_setHandlerFunctionPTR(uSleepOS_PMIPerformer);
  return;
}

#endif /* USLEEP_SUPPORT_TRANSITIONS */

/*
 * uSleepTarget_initialize
 */
void uSleepTarget_initialize(void)
{
  uint32 timerIntN;

  /* set below groups as deferrable */
  timer_group_set_deferrable( &timer_null_group, TRUE );
  timer_group_set_deferrable( &rex_timer_group,  TRUE );
  
  /* Attache and register for the qTimer wakeup */
  DalTimetick_Attach("WakeUpTimer", &g_DALWakeTimerHandle);
  CORE_VERIFY_PTR(g_DALWakeTimerHandle);

  /* Attaching to interrupt controller and registering interrupt handlers */
  CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &g_DALInterruptCtrlHndl));
  CORE_VERIFY_PTR(g_DALInterruptCtrlHndl);

  /* Register a ISR to enable the sleep wakeup timer */
  DalTimetick_GetInterruptVectorNum(g_DALWakeTimerHandle, &timerIntN);

  CORE_DAL_VERIFY(DalInterruptController_RegisterISR(g_DALInterruptCtrlHndl,
                                                     timerIntN,
                                                     (DALIRQ)sleepTarget_wakeupHandler,
                                                     (const DALIRQCtx) 0,
                                                     DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER));

  return;
}



/*
 * uSleepTarget_getRPMStatisticsAddress
 */
void *uSleepTarget_getRPMStatisticsAddress(void)
{
  uint32 rpm_stats_va = 0;

  /* RPM Stats Virtual Address is 112 bytes above
   * the mpm interrupt base virtual address
   */
  rpm_stats_va = (vmpm_getProcMpmBaseVA() - 112);
  return ((void*) rpm_stats_va);
}
