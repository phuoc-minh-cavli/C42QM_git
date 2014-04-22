/*==============================================================================
  FILE:         uSleep_lpr.c

  OVERVIEW:     This file contains the low power modes related support functions 

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/lpr/uSleep_lpr.c#1 $
$DateTime: 2019/02/20 03:28:54 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_lpri.h"
#include "uSleep_target.h"
#include "uATS.h"

/*==============================================================================
                          EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#ifdef DYNAMIC_LPR_CALLBACKS
/*
 * uSleepLPR_registerCallback 
 */ 
usleep_status uSleepLPR_registerCallback(char                   *cbName,
                                         uSleep_lpr_cb          func, 
                                         uSleep_mode_choices    mode,
                                         uSleep_lpr_cb_location location,
                                         void                   *userData, 
                                         uint32                 latency,
                                         uSleep_lpr_cb_attr     attributes)
{
  uSleep_LPR          *synthMode;
  uSleep_dynamic_lpr  *LPRCB;
  uSleep_lpr_callback **list;
  uSleep_lpr_callback *newCB;
  usleep_status       status = USLEEP_SUCCESS;

  CORE_VERIFY(mode < USLEEP_NUMBER_LOW_POWER_MODES);

  /* Get the right list to add to */
  synthMode = &g_uSleepLPR[mode];
  LPRCB     = &synthMode->registered_lprs;

  /* Verify attributes */
  if((TRUE == synthMode->cached) &&
     (0 == (USLEEP_LPR_CB_ATTR_CACHABLE & attributes)))
  {
    return USLEEP_FAILURE;
  }

  /* Create a new element and add it to the end of the CB list */
  newCB = malloc(sizeof(uSleep_lpr_callback));

  if(NULL != newCB)
  {
    if(USLEEP_LPR_CB_ENTER == location)
    {
      list = &LPRCB->enter;
      synthMode->working_latency.enter += latency;
    }
    else
    {
      list = &LPRCB->exit;
      synthMode->working_latency.exit += latency;
    }

    /* Find the end of the list */
    while(NULL != *list)
    {
      list = &((*list)->next);
    }

    if(NULL != cbName)
    {
      newCB->name = cbName;
    }
    else
    {
      newCB->name = "Dynamic LPR";
    }

    newCB->func     = func;
    newCB->userData = userData;
    newCB->next     = NULL;

    *list = newCB;
  }
  else
  {
    status = USLEEP_FAILURE;
  }

  return status;
}
#endif /* DYNAMIC_LPR_CALLBACKS */

/*
 * uSleepLPR_setModeStatus 
 */
void uSleepLPR_setModeStatus(uSleep_mode_choices index, boolean enable)
{
  CORE_VERIFY(index < USLEEP_NUMBER_LOW_POWER_MODES);

  if(TRUE == enable)
  {
    /* Set user enabled flag */
    g_uSleepLPR[index].enabled_flags |= USLEEP_LPR_ENABLE_MASK;
  }
  else
  {
    /* Clear user enabled flag */
    g_uSleepLPR[index].enabled_flags &= ~USLEEP_LPR_ENABLE_MASK;
  }

  /* Invoke uATS to reselect the power mode to enter */
  uATS_SetSignal(USLEEP_SIGNAL_REGISTRY);

  return;
}

/*
 * uSleepLPR_runLPRFunctions 
 */
void uSleepLPR_runLPRFunctions(boolean    enterFunction,
                               uint64     wakeupDeadline,
                               boolean    reEntrant,
                               uSleep_LPR *mode)
{
  if(TRUE == enterFunction)
  {
    if((NULL != mode->enter_func)
       #ifdef DYNAMIC_LPR_CALLBACKS
        || (NULL != mode->registered_lprs.enter)
       #endif
       )
    {
      uSleepTarget_enterLowPowerMode(mode, wakeupDeadline, reEntrant);
    }
  }
  else
  {
    if((NULL != mode->exit_func)
       #ifdef DYNAMIC_LPR_CALLBACKS
        || (NULL != mode->registered_lprs.exit)
       #endif
       )
    {
      uSleepTarget_exitLowPowerMode(mode);
    }
  }

  return;
}

/*
 * uSleepLPR_powerModeBaseInit
 */
void uSleepLPR_powerModeBaseInit(void)
{
  /* Config the base settings for all low power modes */
  uSleepTarget_configBasePowerMode();
  return;
}

