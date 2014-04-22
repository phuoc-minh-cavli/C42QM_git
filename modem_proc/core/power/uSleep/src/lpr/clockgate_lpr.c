/*==============================================================================
  FILE:         clockgate_lpr.c

  OVERVIEW:     This file provides uSleep clock gating LPR functions

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/lpr/clockgate_lpr.c#1 $
$DateTime: 2019/02/20 03:28:54 $
==============================================================================*/
#include "comdef.h"
#include "q6_lpm_config.h"

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepLPR_clockgateEnter 
 */
void uSleepLPR_clockgateEnter(uint64 wakeupTime, boolean reEntrant)
{
  if(TRUE != reEntrant)
  {
    /* Enable clock gating (with no PMI interrupt)*/
    q6LPMConfig_setupModeConfig(RSC_MODE_CLK_GATE, FALSE);
  }

  return;
}

