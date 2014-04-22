/*============================================================================
  @file therm_os.c

  Thermal OS specific functionality.  Task init.

  Copyright (c) 2013,2015 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.tx/6.0/power/thermal-engine/src/os/l4/therm_os.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "posix_errno.h"
#include "comdef.h"
#include "rex.h"
#include "rcinit.h"
#include "task.h"

#include "thermal.h"
#include "sensors.h"
#include "devices.h"
#include "sensors_manager.h"
#include "devices_manager.h"
#include "thermal_config.h"
#include "therm_log.h"

struct thermal_setting_t thermal_settings;
int exit_daemon = 0;
int debug_output = 0;
int minimum_mode = 0;
static char *config_file = NULL;


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
static uint32 gThermMask = TASK_SIG_DEFAULT_OFFLINE | TASK_SIG_DEFAULT_STOP;

extern rex_tcb_type thermal_tcb;

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief therm_task

  Thermal Mitigation task.

  @param ignored: Ignored.
*/
void thermal_task
(
  dword ignored
  /* Parameter received from Rex - ignored */
  /*lint -esym(715,ignored)
 ** Have lint not complain about the ignored parameter 'ignored' which is
 ** specified to make this routine match the template for rex_init().
 */
  )
{
  rex_sigs_type rcvdSigs;   /* signals returned from REX */
  device_clnt_handle vrestrict_clnt = NULL;
  device_clnt_handle mx_min_clnt = NULL;
  device_clnt_handle cx_min_clnt = NULL;
  device_clnt_handle cpr_cold_clnt = NULL;


  therm_log_init();

  therm_log_set_level(THERM_LOG_LEVEL_MAX);

  /*  Initialize the system on this target. */
  devices_manager_init();
  devices_init(minimum_mode);

  /* Vote for vdd_restrict and mx_min until init is complete */
  vrestrict_clnt = devices_manager_reg_clnt("vdd_restriction");
  if (vrestrict_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(vrestrict_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "vdd_restriction");
  }
  mx_min_clnt = devices_manager_reg_clnt("mx_min_request");
  if (mx_min_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(mx_min_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "mx_min_request");
  }
  cx_min_clnt = devices_manager_reg_clnt("cx_min_request");
  if (cx_min_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(cx_min_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "cx_min_request");
  }
  cpr_cold_clnt = devices_manager_reg_clnt("cpr_cold");
  if (cpr_cold_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(cpr_cold_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "cpr_cold_request");
  }

  sensors_manager_init();

  /* Start the task */
  rcinit_handshake_startup(); // required rcinit handshake
  /* Initialize after task start on this target. */
  sensors_init(minimum_mode);

  init_settings(&thermal_settings);

  thermal_monitor_init_data(&thermal_settings);

  load_config(&thermal_settings, config_file);
  thermal_monitor(&thermal_settings);

  /* Cancel vdd_restriction and mx_min request after init. */
  if (vrestrict_clnt != NULL)
  {
    devices_manager_dereg_clnt(vrestrict_clnt);
  }
  if (mx_min_clnt != NULL)
  {
    devices_manager_dereg_clnt(mx_min_clnt);
  }
  if (cx_min_clnt != NULL)
  {
    devices_manager_dereg_clnt(cx_min_clnt);
  }
  if (cpr_cold_clnt != NULL)
  {
    devices_manager_dereg_clnt(cpr_cold_clnt);
  }

  while (1)
  {
    /* Wait for new signals to be received. */
    rcvdSigs = rex_wait(gThermMask);

    if (rcvdSigs & TASK_SIG_DEFAULT_STOP)
    {
      exit_daemon = 1;
      rex_clr_sigs(&thermal_tcb, TASK_SIG_DEFAULT_STOP);
      break;
    }
    rex_clr_sigs(rex_self(), gThermMask);
  }

#if 0
  devices_release();
  devices_manager_release();
  sensors_release();
  sensors_manager_release();
#endif
}

