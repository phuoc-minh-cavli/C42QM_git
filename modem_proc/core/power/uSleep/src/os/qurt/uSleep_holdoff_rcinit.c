/*==============================================================================
  FILE:         sleep_holdoff.c
  
  OVERVIEW:     This file provides functions for supporting sleep holdoff 
                feature which will prevent processor to go in to any low
                power mode until all clients are ready.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/os/qurt/uSleep_holdoff_rcinit.c#1 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "DALSys.h"
//#include "sleepActive.h"
#include "rcinit.h"
#include "rcevt_dal.h"
#include "uATS.h"
#include "uSleep_os.h"
#include "uSleep_log.h"

#ifdef SLEEP_TEST_ENABLE_LATENCY
#include "sleep_osi.h"
#include "sleep_testi.h"
#endif

/*==============================================================================
                               EXTERNAL VARIABLES
 =============================================================================*/
extern volatile uint8 g_uSleepAllowLowPowerModes;

/*==============================================================================
                               INTERNAL VARIABLES
 =============================================================================*/
/* DAL event handle used by RCInit framework to signal that it has finished
 * initialization. */
DALSYSEventHandle   g_uSleepRCInitCompleteEventHandle;

/* Signal object which will be registered with RCInit framework for 
 * initialization completion event. */
RCEVT_SIGEX_SIGDAL  g_uSleepRCInitCompleteSignal;

/*==============================================================================
                               INTERNAL FUNCTIONS
 =============================================================================*/
/** 
 * sleepHoldoff_releaseCB
 * 
 * @brief Callback function that gets called through DAL event which will be
 *        generated as a signal when RCInit framework finishes initialization. Once 
 *        called, it will issue request to release holdoff on Sleep on behalf of 
 *        systems that gets initialized via RCInit. 
 *
 * @param cbCtxt: Call back context used while registering it. 
 * @param val: Param used by Event generator 
 * @param payload: Payload passed by Event generator
 * @param payloadSize: Size of the above payload
 *
 * @note: None of the parameters passed is used for now. They are for
 *        complying with callback prototype.
 *
 * @return Nothing (to comply with callback prototype)
 */
static void *uSleepHoldoff_releaseCB(void    *cbCtxt,
                                    uint32  val,
                                    void    *payload,
                                    uint32  payloadSize)
{
  /* Releasing hold off from RCinit side if global sleep flag set to allow
   * low power modes. */
  if(g_uSleepAllowLowPowerModes > 0)
  {
    uSleepOS_configIdleMode(USLEEP_OS_IDLE_MODE_DEFAULT);
    
    uSleepLog_printf(0, "ATS: RCinit complete, allowing normal operation");
  }

  return NULL;
}

/*==============================================================================
                               GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * sleepHoldoff_registerRCinitCompletionEvent
 * 
 * @brief Registers an initialization completion event and corresponding callback 
 *        with RCInit framework.
 */
void uSleepHoldoff_registerRCinitCompletionEvent(void)
{
  RCEVT_HANDLE retHandle;

  /* Creating a DAL event */
  CORE_DAL_VERIFY(DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL | DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                                     &g_uSleepRCInitCompleteEventHandle, NULL));
  
  /* Set up a call back for the DAL event created above */
  CORE_DAL_VERIFY(DALSYS_SetupCallbackEvent(g_uSleepRCInitCompleteEventHandle, 
                                            uSleepHoldoff_releaseCB, NULL));
  
  /* Registering the DAL event as a signal with RCInit framework which will 
   * be generated after completing intialization */
  g_uSleepRCInitCompleteSignal.signal = g_uSleepRCInitCompleteEventHandle;

  retHandle = rcevt_register_sigex_name(RCINIT_RCEVT_INITGROUPS,
                                        RCEVT_SIGEX_TYPE_SIGDAL,
                                        &g_uSleepRCInitCompleteSignal);

  CORE_VERIFY(RCEVT_NULL != retHandle);

  return;
}

