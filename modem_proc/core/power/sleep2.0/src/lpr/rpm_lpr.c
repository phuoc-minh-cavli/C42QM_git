/*==============================================================================
  FILE:         rpm_lpr.c

  OVERVIEW:     This file provides the LPR definition for the RPM  
                low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2011-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.10/power/sleep2.0/src/lpr/rpm_lpr.c#6 $
$DateTime: 2019/07/23 00:25:38 $
==============================================================================*/
#include <stdbool.h>
#include "HALcomdef.h"
#include "sleepi.h"
#include "sleep_lpr.h"
#include "rpm.h"
#include "rpmclient.h"
#include "kvp.h"
#include "CoreVerify.h"
#include "smd.h"
#include "vmpm_utils.h"
#include "sleep_log.h"
#include "sleep_target.h"
/*==============================================================================
                              Type def
 =============================================================================*/

#define SLEEP_KEY_KHZ                            0x007a484b // value :any, KHz

/*==============================================================================
                              GLOBAL VARIABLES
 =============================================================================*/
volatile uint32 g_BIMCFreq;
unsigned int    sleepClkIdxBimc = 0; /*TBD*/
/*==============================================================================
                              EXTERNAL VARIABLES
 =============================================================================*/
extern kvp_t        *pSleepBimcKvp;
extern npa_query_handle g_SleepBimcQueryHandle;
extern npa_client_handle g_sleepBimcClientHandle;

/*==============================================================================
                              INTERNAL VARIABLES
 =============================================================================*/
static uint32 g_nasResult;
static uint32 g_ssResult;

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * RPMSyncLPR_putClkBimcVote
 *
 * @brief Function to vote for /clk/bimc freq. 
 */

void RPMSyncLPR_putClkBimcVote(rpm_set_type rpm_set, const void *value )
{

  kvp_clear(pSleepBimcKvp);
  kvp_reset(pSleepBimcKvp);
  kvp_put(pSleepBimcKvp, SLEEP_KEY_KHZ, 4, value);
  rpm_post_request(rpm_set, RPM_CLOCK_2_REQ, sleepClkIdxBimc, pSleepBimcKvp); 
 
} 
 
/**
 * RPMSyncLPR_enter
 *
 * @brief Enter function for RPM synchronization low power mode.
 */
void RPMSyncLPR_enter( uint64 wakeup_tick )
{
	
  
  uint32          sleepDdrClk = 336000; /* NOM value for DDR  clock: TODO: define a macro*/
  npa_query_type          qres;

  /* Get current bimc frequency before going to sleep. */
  CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query(g_SleepBimcQueryHandle,
                                             NPA_QUERY_CURRENT_STATE,
                                             &qres));
  g_BIMCFreq = qres.data.state;  
  
  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                    "original bimc freq before sleep: %d", g_BIMCFreq);
  
  /* Indicate we are doing an RPM assisted mode */
  sleepTarget_setRPMAssistedMode(TRUE); 

  sleepTarget_setRpmHandshake(TRUE);
  
  /* vote for NOM - 336 MHz for bimc clk only if BIMC clock freq is less than or equal to SVS -168 MHz */
  if( g_BIMCFreq <= 168000)
  {
    RPMSyncLPR_putClkBimcVote(RPM_NEXT_ACTIVE_SET, (void *)&sleepDdrClk);
  }	
 
  /* Tell RPM the wakeup time */
  vmpm_SetWakeupTimetick(wakeup_tick);
  g_ssResult = rpm_force_sync(RPM_SLEEP_SET);

  sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_INFO, SLEEP_SET_SEND_NUM_ARGS,
                       SLEEP_SET_SEND_STR, SLEEP_SET_SEND,
                       ULOG64_DATA(wakeup_tick) );

  g_nasResult = rpm_force_sync(RPM_NEXT_ACTIVE_SET);

  if(g_ssResult || g_nasResult)
  {
    rpm_mask_interrupt(true);
  }

  return;
}

/**
 * RPMSyncLPR_exit
 *
 * @brief Exit function for RPM synchronization low power mode.
 */
void RPMSyncLPR_exit( void )
{
  	
  /*restore the original DDR clock value as before going to sleep */  
  if( g_BIMCFreq <= 168000)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                    " bimc freq vote placed after sleep: %d", g_BIMCFreq);
    RPMSyncLPR_putClkBimcVote(RPM_ACTIVE_SET, (void *)&g_BIMCFreq);
  }	
  
  /* Ensure churn_queue uses g_nasResult if valid, else g_ssResult */
  rpm_churn_queue(g_nasResult?g_nasResult:g_ssResult);

  if(g_ssResult || g_nasResult)
  {
    rpm_mask_interrupt(false);
  }

  sleepTarget_setRpmHandshake(FALSE);
   
  /* Unset RPM assisted mode */
  sleepTarget_setRPMAssistedMode(FALSE); 

  return;
}

