/*==============================================================================
  FILE:         uSleep_lpr_modes.c

  OVERVIEW:     This file contains the low power modes supported in island mode
               

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/lpr/uSleep_lpr_default.c#1 $
$DateTime: 2019/02/20 03:28:54 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_lpri.h"

/*==============================================================================
                             FORWARD DECLRATIONS
 =============================================================================*/
void uSleepLPR_powerModeBaseInit(void);
void uSleepLPR_clockgateEnter(uint64 wakeupTime, boolean reEntrant);

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Always enabled, default low power mode when no other mode can be chosen */
uSleep_LPR g_uSleepDefaultLPR = {"Clock Gate",                 /* LPR name */
                                 {0,0},                        /* Seed enter, exit latency */
                                 0,                            /* Backoff latency */
                                 uSleepLPR_powerModeBaseInit,  /* Init function */
                                 uSleepLPR_clockgateEnter,     /* Enter & exit functions for the LPR */
                                 NULL,
                                 TRUE,                         /* Cacheable flag */
                                 USLEEP_LPR_ENABLED};          /* Default internal enable flag */

