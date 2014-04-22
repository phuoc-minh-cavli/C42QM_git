/*============================================================================
  FILE:         lpr_definition_cpu_vdd.c

  OVERVIEW:     This file provides the sleep LPR definition for the
                QDSP6V4 ALL WAIT low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2011-2018 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

#include <stdint.h>
#include "HALcomdef.h"
#include <stdbool.h>
#include "sleep_lpr.h"
#include "CoreVerify.h"
#include "kvp.h"
#include "rpmclient.h"
#include "smd.h"
#include "mpm_utils.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "sleep_statsi.h"

/*=============================================================================
 *                        INTERNAL VARIABLES AND MACROS
 *============================================================================*/
#ifndef inp64
#define inp64(port)       (*((volatile uint64 *)(port)))
#endif

/*============================================================================
 *                      FUNCTIONS FOR RPM SYNC LPR'S MODES
 *============================================================================*/

bool rpm_is_up(void);

static unsigned int nas_result;
static unsigned int ss_result;

/* Flag to avoid rpm communication by spm (and hence set switching)
 * in case rpm is not running during bringup. */
int sleep_perform_rpm_comm = 0;

int rpm_assisted_power_collapse = 0;

void rpm_sync_init( void )
{
  if( rpm_is_up() )
  {
    sleep_perform_rpm_comm = 1;
  }

  return;
}

void rpm_sync_enter( uint64_t wakeup_tick )
{
  /* If RPM is not up (during bringup) just return */
  if( !sleep_perform_rpm_comm )
  {
    return;
  }

  sleep_target_set_rpm_assisted_mode( TRUE );

  mpm_set_wakeup_timetick( wakeup_tick );

  ss_result = rpm_force_sync(RPM_SLEEP_SET);
  
  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_INFO, SLEEP_SET_SEND_NUM_ARGS,
                         SLEEP_SET_SEND_STR, SLEEP_SET_SEND,
                         ULOG64_DATA(wakeup_tick) );

  nas_result = rpm_force_sync(RPM_NEXT_ACTIVE_SET);

  if(ss_result || nas_result)
  {
    CORE_VERIFY(smd_disable_intr(SMEM_RPM) == SMD_STATUS_SUCCESS);
  }
   
  rpm_assisted_power_collapse = 1;

  return;
}

void rpm_sync_exit( void )
{
  RPM_master_stats *rpm_stats;

  if( !sleep_perform_rpm_comm )
  {
    return ;
  }

  rpm_stats = sleepStats_getRpmDataPtr();
  CORE_VERIFY_PTR( rpm_stats );

  /* Ensure churn_queue uses nas_result if valid, else ss_result */
  rpm_churn_queue(nas_result?nas_result:ss_result);

  if(ss_result || nas_result)
  {
    CORE_VERIFY(smd_enable_intr(SMEM_RPM) == SMD_STATUS_SUCCESS);
  }

  /* Set wakeup reason as given from RPM */
  sleepStats_putMiscData(inpdw(&rpm_stats->wakeup_reason), 
                         SLEEP_STATS_MISC_WAKEUP_REASON_TYPE);

  /* In RPM assisted modes, the wakeup time will come from RPM. Overwrite the 
   * previously put standalone PC time */
  sleepStats_putLprTimeData(inp64(&rpm_stats->bringup_ack), 
                            SLEEP_STATS_TIME_WAKEUP_TYPE);

  sleep_target_set_rpm_assisted_mode( FALSE );
  rpm_assisted_power_collapse = 0;

  return;
}
