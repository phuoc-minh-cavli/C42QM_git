/*==============================================================================
  FILE:         rsc_target.c

  OVERVIEW:     This file implements any target specific functions for rsc.

  DEPENDENCIES: None

  NOTES:        This needs to be refactored once full DeviceCfg (or equivalent)
                support is present.
                
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "comdef.h"
#include "rsc.h"
#include "rsc_internal.h"

/*=======================================================================
 *                             EXTERNAL VARIABLES
 *======================================================================*/
/* Externed variables containing rsc configuration information for different
 * versions. */
extern rsc_t  g_rscInstances[];
extern uint32 g_rscInstCount;
extern rsc_t  *g_rscs;
extern uint32 g_rscCount;

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/
/* 
 * rsc_target_init
 */
void rsc_target_init(void)
{
  /* Add version specific initialization if any */
  g_rscs      = g_rscInstances;
  g_rscCount  = g_rscInstCount;

  return;
}
